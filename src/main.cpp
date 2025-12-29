#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <chrono>

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

bool parse(int argc, char** argv, int idx, bool defval=false) {
    if (idx >= argc) return defval;
    std::string s = argv[idx];
    if (s.empty()) return defval;

    // lowercase
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return (char)std::tolower(c); });

    if (s == "true")  return true;
    if (s == "false") return false;

    return defval;
}

// Pipeline : points -> delaunay -> mesh -> grid -> raster -> ppm
static void run_pipeline(const std::string& out_ppm, const std::vector<Point3D>& pts, const BBox2D& bbox, double zmin, double zmax, std::size_t width, bool ombrage){
    std::vector<double> coords;
    std::vector<double> alts;
    coords.reserve(pts.size() * 2);
    alts.reserve(pts.size());

    for (const auto& p : pts) {
        coords.push_back(p.x);
        coords.push_back(p.y);
        alts.push_back(p.z);
    }

    std::vector<std::size_t> tris;
    {
        Timer t("Delaunay");
        delaunator::Delaunator d(coords);
        tris = d.triangles;
    }

    Mesh2D mesh(coords, tris, alts);

    Grid grid(mesh, bbox, 1000, 1000);
    TriangleLocator locator(mesh, std::move(grid));

    Rasterizer rast(locator, bbox, zmin, zmax);
    std::size_t height = 0;
    auto img = rast.render_p6_color(width, height, ombrage, -12.0, 45.0);

    PPM::write_p6(out_ppm, width, height, img);
    std::cout << "Enregistré sous : " << out_ppm << " (" << width << "x" << height << ")\n";
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Utilisation : " << argv[0]
                  << " <fichier_mnt> <largeur_pixels> [use_fourier] [use_ombrage]\n"
                  << "Exemples:\n"
                  << "  " << argv[0] << " Guerledan.txt 800\n"
                  << "  " << argv[0] << " Guerledan.txt 800 true\n"
                  << "  " << argv[0] << " Guerledan.txt 800 true false\n";
        return EXIT_FAILURE;
    }

    const std::string filepath = argv[1];
    const std::size_t width = static_cast<std::size_t>(std::atoi(argv[2]));


    const bool USE_FOURIER  = parse(argc, argv, 3, false);
    const bool USE_OMBRAGE  = parse(argc, argv, 4, false);

    std::cout << "fourier=" << (USE_FOURIER ? "true" : "false")
              << " ombrage=" << (USE_OMBRAGE ? "true" : "false") << "\n";

    // 1) Lecture
    TerrainData terrain;
    {
        Timer t("Lecture fichier");
        terrain.load_data_from_file(filepath);
    }
    std::cout << "Lecture OK : " << terrain.size() << " points\n";

    // 2) Projection
    Projector projector;
    TerrainProjected proj(terrain, projector);

    BBox2D bbox{proj.min_x(), proj.min_y(), proj.max_x(), proj.max_y()};

    // 3) Points projetés (x,y,z)
    std::vector<Point3D> pts_proj;
    pts_proj.reserve(terrain.points().size());
    {
        Timer t("Assemblage points projetes (x,y,z)");
        const auto& P = proj.points();
        const auto& G = terrain.points(); // altitudes
        for (std::size_t i = 0; i < P.size(); ++i) {
            pts_proj.push_back({P[i].x, P[i].y, G[i].alt});
        }
    }

    // 4) Choix points pour Delaunay : direct ou Fourier
    std::vector<Point3D> pts_for_delaunay = pts_proj;

    if (USE_FOURIER) {
        FourierPreprocess::Params p;
        p.grid_scale  = 1.0;
        p.fill_iters  = 4;
        p.sigma_px    = 2.0;
        p.sample_step = 2;
        p.pow2_grid   = true;

        {
            Timer t("Fourier");
            FourierPreprocess fp(p);
            pts_for_delaunay = fp.run(pts_proj, bbox, width);

            auto info = fp.last_grid();
            std::cout << "Fourier grid: " << info.gw << "x" << info.gh
                      << " step=" << p.sample_step
                      << " pts_out=" << pts_for_delaunay.size() << "\n";
        }
    }

    // 5) Raster
    const std::string out = USE_FOURIER? (USE_OMBRAGE ? "mnt_avec_fourier_avec_ombrage.ppm" : "mnt_avec_fourier_sans_ombrage.ppm")
    : (USE_OMBRAGE ? "mnt_sans_fourier_avec_ombrage.ppm" : "mnt_sans_fourier_sans_ombrage.ppm");

    run_pipeline(out, pts_for_delaunay, bbox, terrain.min_alt(), terrain.max_alt(), width, USE_OMBRAGE);

    return 0;
}
