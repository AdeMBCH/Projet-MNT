#include <iostream>
#include "terraindata.hpp"
#include "projector.hpp"
#include "terrainprojected.hpp"
#include "delaunator.hpp"

#include "mesh2D.hpp"
#include "grid.hpp"
#include "trianglelocator.hpp"
#include "rasterise.hpp"
#include "ppm.hpp"

#include "fourier.hpp" 



#include <chrono>

struct Timer {
    std::string name;
    std::chrono::high_resolution_clock::time_point t0;
    Timer(const std::string& n) : name(n), t0(std::chrono::high_resolution_clock::now()) {}
    ~Timer() {
        auto t1 = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
        std::cout << name << " : " << ms << " ms\n";
    }
};


// int main(int argc, char** argv)
// {
//     if (argc != 3) {
//         std::cerr << "Utilisation : " << argv[0] << " <fichier_mnt> <largeur_pixels>\n";
//         std::cerr << "Exemple: " << argv[0] << " Guerledan.txt 800\n";
//         return EXIT_FAILURE;
//     }

//     // 1) Lecture
//     TerrainData terrain;
//     //const std::string filepath = "Guerledan.txt";
//     const std::string filepath = argv[1];
//     terrain.load_data_from_file(filepath);

//     std::cout << "Lecture OK : " << terrain.size() << " points\n";

//     // 2) Projection
//     Projector projector;
//     TerrainProjected proj(terrain, projector);

//     // 3) coords + alts alignés (même ordre)
//     std::vector<double> coords;
//     coords.reserve(proj.points().size() * 2);
//     for (const auto& p : proj.points()) {
//         coords.push_back(p.x);
//         coords.push_back(p.y);
//     }

//     std::vector<double> alts;
//     alts.reserve(terrain.points().size());
//     for (const auto& gp : terrain.points()) {
//         alts.push_back(gp.alt);
//     }

//     // 4) Triangulation (non OO)
//     delaunator::Delaunator d(coords);

//     // 5) Passage en OO
//     std::vector<std::size_t> tris = d.triangles;
//     Mesh2D mesh(coords, tris, alts);

//     BBox2D bbox{proj.min_x(), proj.min_y(), proj.max_x(), proj.max_y()};

//     // 6) Index spatial : grille uniforme
//     Grid grid(mesh, bbox, 1000, 1000);
//     TriangleLocator locator(mesh, std::move(grid));

//     // 7) Rasterisation
//     Rasterizer rast(locator, bbox, terrain.min_alt(), terrain.max_alt());
//     std::size_t height = 0;
//     //const std::size_t width = 1200;
//     const std::size_t width = static_cast<std::size_t>(std::atoi(argv[2]));
//     auto img = rast.render_p6_color(width, height, false, 315.0, 45.0);


//     // 8) Écriture PPM
//     PPM::write_p6("mnt.ppm", width, height, img);

//     std::cout << "OK: mnt.ppm (" << width << "x" << height << ")\n";
//     return 0;
// }


static void run_pipeline(const std::string& tag,
                         const std::vector<Point3D>& pts,
                         const BBox2D& bbox,
                         double zmin, double zmax,
                         std::size_t width)
{
    std::cout << "\n=== Pipeline " << tag << " ===\n";

    std::vector<double> coords;
    std::vector<double> alts;
    coords.reserve(pts.size() * 2);
    alts.reserve(pts.size());

    for (const auto& p : pts) {
        coords.push_back(p.x);
        coords.push_back(p.y);
        alts.push_back(p.z);
    }

    // 1) Delaunay (PAS d'affectation)
    std::vector<std::size_t> tris;
    {
        Timer t("Delaunay");
        delaunator::Delaunator d(coords);
        tris = d.triangles; // copie -> OK, d va mourir
    }

    // 2) Mesh OO
    Mesh2D mesh(coords, tris, alts);

    // 3) Index + locator
    Grid grid(mesh, bbox, 1000, 1000);
    TriangleLocator locator(mesh, std::move(grid));

    // 4) Raster
    Rasterizer rast(locator, bbox, zmin, zmax);
    std::size_t height = 0;
    auto img = rast.render_p6_color(width, height, false, 315.0, 45.0);

    // 5) Output
    const std::string out = (tag == "SANS Fourier") ? "mnt_no_fourier.ppm" : "mnt_with_fourier.ppm";
    PPM::write_p6(out, width, height, img);

    std::cout << "OK: " << out << " (" << width << "x" << height << ")\n";
}


#include <cstdlib> // atoi
#include <vector>
#include <string>

// Petit helper: construit coords/alts depuis un vecteur Point3D
static void build_coords_alts(const std::vector<Point3D>& pts,
                              std::vector<double>& coords,
                              std::vector<double>& alts)
{
    coords.clear();
    alts.clear();
    coords.reserve(pts.size() * 2);
    alts.reserve(pts.size());

    for (const auto& p : pts) {
        coords.push_back(p.x);
        coords.push_back(p.y);
        alts.push_back(p.z);
    }
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cerr << "Utilisation : " << argv[0] << " <fichier_mnt> <largeur_pixels>\n";
        std::cerr << "Exemple: " << argv[0] << " Guerledan.txt 800\n";
        return EXIT_FAILURE;
    }

    const std::string filepath = argv[1];
    const std::size_t width = static_cast<std::size_t>(std::atoi(argv[2]));

    // ====== 1) Lecture ======
    TerrainData terrain;
    {
        Timer t("Lecture fichier");
        terrain.load_data_from_file(filepath);
    }
    std::cout << "Lecture OK : " << terrain.size() << " points\n";

    // ====== 2) Projection ======
    Projector projector;
    TerrainProjected proj(terrain, projector);

    BBox2D bbox{proj.min_x(), proj.min_y(), proj.max_x(), proj.max_y()};

    // Construction d’un vecteur (x,y,z) projeté pour Fourier (et pour le pipeline normal)
    std::vector<Point3D> pts_proj;
    pts_proj.reserve(terrain.points().size());
    {
        Timer t("Assemblage points projetes (x,y,z)");
        const auto& P = proj.points();
        const auto& G = terrain.points();
        // hypothèse : même ordre et même taille
        for (std::size_t i = 0; i < P.size(); ++i) {
            pts_proj.push_back({P[i].x, P[i].y, G[i].alt});
        }
    }

    run_pipeline("SANS Fourier", pts_proj, bbox, terrain.min_alt(), terrain.max_alt(), width);

    // Fourier
    FourierPreprocess::Params p;
    p.grid_scale  = 1.0;
    p.fill_iters  = 4;
    p.sigma_px    = 2.0;
    p.sample_step = 2;
    p.pow2_grid   = true;

    std::vector<Point3D> pts_fourier;
    {
        Timer t("Fourier preprocess");
        FourierPreprocess fp(p);
        pts_fourier = fp.run(pts_proj, bbox, width);
        auto info = fp.last_grid();
        std::cout << "Fourier grid: " << info.gw << "x" << info.gh
                << " step=" << p.sample_step
                << " pts_out=" << pts_fourier.size() << "\n";
    }

    run_pipeline("AVEC Fourier", pts_fourier, bbox, terrain.min_alt(), terrain.max_alt(), width);


    return 0;
}