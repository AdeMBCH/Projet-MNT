// #include <iostream>
// #include <proj.h>
// #include "terraindata.hpp"

// using namespace std;

// int main()
// {
//     const std::string filepath = "rade.txt";

//     try {
//         TerrainData terrain;
//         terrain.load_data_from_file(filepath);

//         std::cout << "Nombre de points : " << terrain.size() << "\n";
//         std::cout << "Latitude  : [" << terrain.min_lat() << ", " << terrain.max_lat() << "]\n";
//         std::cout << "Longitude : [" << terrain.min_lon() << ", " << terrain.max_lon() << "]\n";
//         std::cout << "Altitude  : [" << terrain.min_alt() << ", " << terrain.max_alt() << "]\n";
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Erreur : " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }

//     return EXIT_SUCCESS;
// }

#include <iostream>
#include "terraindata.hpp"
#include "projector.hpp"
#include "terrainprojected.hpp"
#include "delaunator.hpp"

int main()
{
    TerrainData terrain;
    const std::string filepath = "rade.txt";

    terrain.load_data_from_file(filepath);
    std::cout << "Lecture OK : " << terrain.size() << " points\n";

    Projector projector;
    TerrainProjected proj(terrain, projector);

    std::cout << "Projection OK.\n";
    std::cout << "Projetion X : [" << proj.min_x() << ", " << proj.max_x() << "]\n";
    std::cout << "Projection Y : [" << proj.min_y() << ", " << proj.max_y() << "]\n";

    return 0;
}

