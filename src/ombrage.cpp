#include "ombrage.hpp"
#include <algorithm>
#include <cmath>

std::vector<double> Ombrage::compute(const std::vector<double>& z,
                                       std::size_t w, std::size_t h,
                                       double dx, double dy,
                                       double azimuth_deg,
                                       double altitude_deg)
{
    std::vector<double> shade(w * h, 0.0);

    const double az = deg2rad(azimuth_deg);
    const double alt = deg2rad(altitude_deg);

    // Direction lumière (Lambert)
    const double lx = std::sin(az) * std::cos(alt);
    const double ly = std::cos(az) * std::cos(alt);
    const double lz = std::sin(alt);

    auto at = [&](std::size_t x, std::size_t y) -> double {
        return z[y * w + x];
    };

    for (std::size_t y = 1; y + 1 < h; ++y) {
        for (std::size_t x = 1; x + 1 < w; ++x) {
            const double zL = at(x - 1, y);
            const double zR = at(x + 1, y);
            const double zD = at(x, y + 1);
            const double zU = at(x, y - 1);

            // gradients (différences centrales)
            const double dzdx = (zR - zL) / (2.0 * dx);
            const double dzdy = (zD - zU) / (2.0 * dy);

            // normale (non normalisée) : (-dzdx, -dzdy, 1)
            double nx = -dzdx;
            double ny = -dzdy;
            double nz = 1.0;

            const double norm = std::sqrt(nx*nx + ny*ny + nz*nz);
            nx /= norm; ny /= norm; nz /= norm;

            // intensité lambertienne
            double s = nx * lx + ny * ly + nz * lz;
            s = std::clamp(s, 0.0, 1.0);

            // légère courbe gamma pour contraster
            s = std::pow(s, 0.9);

            shade[y * w + x] = s;
        }
    }

    // bords: copie proche (simple)
    for (std::size_t x = 0; x < w; ++x) {
        shade[0 * w + x] = shade[1 * w + x];
        shade[(h - 1) * w + x] = shade[(h - 2) * w + x];
    }
    for (std::size_t y = 0; y < h; ++y) {
        shade[y * w + 0] = shade[y * w + 1];
        shade[y * w + (w - 1)] = shade[y * w + (w - 2)];
    }

    return shade;
}

double Ombrage::deg2rad(double d) { 
    return d * 3.14159265358979323846 / 180.0; 
}
