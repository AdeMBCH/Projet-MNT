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

int main()
{
    // 1) Lecture
    TerrainData terrain;
    const std::string filepath = "rade.txt";

    terrain.load_data_from_file(filepath);
    std::cout << "Lecture OK : " << terrain.size() << " points\n";

    // 2) Projection
    Projector projector;
    TerrainProjected proj(terrain, projector);

    // 3) coords + alts alignés (même ordre)
    std::vector<double> coords;
    coords.reserve(proj.points().size() * 2);
    for (const auto& p : proj.points()) {
        coords.push_back(p.x);
        coords.push_back(p.y);
    }

    std::vector<double> alts;
    alts.reserve(terrain.points().size());
    for (const auto& gp : terrain.points()) {
        alts.push_back(gp.alt);
    }

    // 4) Triangulation (non OO)
    delaunator::Delaunator d(coords);

    // 5) Passage en OO
    std::vector<std::size_t> tris = d.triangles;
    Mesh2D mesh(coords, tris, alts);

    BBox2D bbox{proj.min_x(), proj.min_y(), proj.max_x(), proj.max_y()};

    // 6) Index spatial : grille uniforme
    // Pour un MNT dense, commence avec 400x400 ou 600x600 selon RAM.
    Grid grid(mesh, bbox, 500, 500);
    TriangleLocator locator(mesh, std::move(grid));

    // 7) Rasterisation
    Rasterizer rast(locator, bbox, terrain.min_alt(), terrain.max_alt());
    std::size_t height = 0;
    const std::size_t width = 1200; // paramètre user plus tard
    auto img = rast.render_p6_color(width, height, false, 315.0, 45.0);


    // 8) Écriture PPM
    PPM::write_p6("mnt_color.ppm", width, height, img);

    std::cout << "OK: mnt.ppm (" << width << "x" << height << ")\n";
    return 0;
}
