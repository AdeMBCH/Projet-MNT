#ifndef RASTERISE_HPP
#define RASTERISE_HPP

#include <vector>
#include <cstdint>
#include "mesh2D.hpp"
#include "trianglelocator.hpp"
#include "colormap.hpp"

class Rasterizer {
public:
    Rasterizer(const TriangleLocator& locator,
               BBox2D bbox,
               double zmin,
               double zmax);

    // width imposée, height calculée
    std::vector<std::uint8_t> render_p6_color(std::size_t width,std::size_t& out_height,bool hillshade_enabled = true,double azimuth_deg = 315.0,double altitude_deg = 45.0) const;

private:
    const TriangleLocator& m_locator;
    BBox2D m_bbox;
    HaxbyColorMap m_cmap;
    double m_zmin;
    double m_zmax;
};

#endif