#include "rasterise.hpp"
#include <cmath>
#include <stdexcept>
#include "ombrage.hpp"

Rasterizer::Rasterizer(const TriangleLocator& locator, BBox2D bbox, double zmin, double zmax): m_locator(locator), m_bbox(bbox), m_cmap(zmin, zmax){}

std::vector<std::uint8_t> Rasterizer::render_p6_color(std::size_t width,
                                                      std::size_t& out_height,
                                                      bool ombrage_enabled,
                                                      double azimuth_deg,
                                                      double altitude_deg) const
{
    if (width == 0) throw std::runtime_error("Rasterizer: width == 0.");

    const double bbox_w = m_bbox.maxx - m_bbox.minx;
    const double bbox_h = m_bbox.maxy - m_bbox.miny;
    if (bbox_w <= 0 || bbox_h <= 0) throw std::runtime_error("Rasterizer: bbox invalide.");

    out_height = static_cast<std::size_t>(std::llround((bbox_h / bbox_w) * static_cast<double>(width)));
    if (out_height == 0) out_height = 1;

    const double dx = bbox_w / static_cast<double>(width);
    const double dy = bbox_h / static_cast<double>(out_height);

    // 1) Raster Z (double) + masque validité
    std::vector<double> zgrid(width * out_height, 0.0);
    std::vector<std::uint8_t> mask(width * out_height, 0);

    for (std::size_t j = 0; j < out_height; ++j) {
        const double y = m_bbox.maxy - (static_cast<double>(j) + 0.5) * dy;
        for (std::size_t i = 0; i < width; ++i) {
            const double x = m_bbox.minx + (static_cast<double>(i) + 0.5) * dx;

            auto z_opt = m_locator.interpolate(x, y);
            const std::size_t id = j * width + i;

            if (z_opt) {
                zgrid[id] = *z_opt;
                mask[id] = 1;
            }
        }
    }

    // 2) Hillshade (optionnel)
    std::vector<double> shade;
    if (ombrage_enabled) {
        shade = Ombrage::compute(zgrid, width, out_height, dx, dy, azimuth_deg, altitude_deg);
    } else {
        shade.assign(width * out_height, 1.0);
    }

    // 3) Couleur + shading
    std::vector<std::uint8_t> img(width * out_height * 3, 0);

    for (std::size_t j = 0; j < out_height; ++j) {
        for (std::size_t i = 0; i < width; ++i) {
            const std::size_t id = j * width + i;
            RGB col;

            if (!mask[id]) {
                col = {0, 0, 0}; // hors hull -> noir
            } else {
                RGB col = m_cmap(zgrid[id]);
                double s = 0.35 + 0.65 * shade[id];   // si hillshade activé
                col = ColorMapHaxby::shade(col, s);
            }

            const std::size_t idx = 3 * id;
            img[idx + 0] = col.r;
            img[idx + 1] = col.g;
            img[idx + 2] = col.b;
        }
    }

    return img;
}