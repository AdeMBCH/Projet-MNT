#include "ppm.hpp"
#include <fstream>
#include <stdexcept>

void PPM::write_p6(const std::string& filename,
                         std::size_t width,
                         std::size_t height,
                         const std::vector<std::uint8_t>& rgb)
{
    if (rgb.size() != width * height * 3) {
        throw std::runtime_error("PPMWriter: buffer RGB de taille incorrecte.");
    }

    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("PPMWriter: impossible d'ouvrir le fichier de sortie.");
    }

    // En-tête PPM P6
    ofs << "P6\n" << width << " " << height << "\n255\n";
    ofs.write(reinterpret_cast<const char*>(rgb.data()),
              static_cast<std::streamsize>(rgb.size()));
}
