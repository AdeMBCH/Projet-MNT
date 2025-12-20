#include "grid.hpp"
#include <algorithm>
#include <cmath>

Grid::Grid(const Mesh2D& mesh, BBox2D bbox, std::size_t nx, std::size_t ny): m_mesh(mesh), m_bbox(bbox), m_nx(nx), m_ny(ny)
{
    if (m_nx < 1) m_nx = 1;
    if (m_ny < 1) m_ny = 1;

    m_dx = (m_bbox.maxx - m_bbox.minx) / static_cast<double>(m_nx);
    m_dy = (m_bbox.maxy - m_bbox.miny) / static_cast<double>(m_ny);

    // éviter division par zéro
    if (m_dx <= 0) m_dx = 1.0;
    if (m_dy <= 0) m_dy = 1.0;

    m_cells.resize(m_nx * m_ny);

    // Insertion triangles
    for (std::size_t ti = 0; ti < m_mesh.triangle_count(); ++ti) {
        const BBox2D tb = m_mesh.triangle_bbox(ti);

        const long ix0 = static_cast<long>(std::floor((tb.minx - m_bbox.minx) / m_dx));
        const long ix1 = static_cast<long>(std::floor((tb.maxx - m_bbox.minx) / m_dx));
        const long iy0 = static_cast<long>(std::floor((tb.miny - m_bbox.miny) / m_dy));
        const long iy1 = static_cast<long>(std::floor((tb.maxy - m_bbox.miny) / m_dy));

        const std::size_t cx0 = clamp_index(ix0, m_nx);
        const std::size_t cx1 = clamp_index(ix1, m_nx);
        const std::size_t cy0 = clamp_index(iy0, m_ny);
        const std::size_t cy1 = clamp_index(iy1, m_ny);

        for (std::size_t cy = cy0; cy <= cy1; ++cy) {
            for (std::size_t cx = cx0; cx <= cx1; ++cx) {
                m_cells[cell_index(cx, cy)].push_back(ti);
            }
        }
    }
}

std::size_t Grid::cell_index(std::size_t ix, std::size_t iy) const { 
    return iy * m_nx + ix; 
}

std::size_t Grid::clamp_index(long v, std::size_t maxv) const {
    if (v < 0) return 0;
    const long hi = static_cast<long>(maxv - 1);
    if (v > hi) return static_cast<std::size_t>(hi);
    return static_cast<std::size_t>(v);
}

std::pair<std::size_t,std::size_t> Grid::cell_of(double x, double y) const {
    const long ix = static_cast<long>(std::floor((x - m_bbox.minx) / m_dx));
    const long iy = static_cast<long>(std::floor((y - m_bbox.miny) / m_dy));
    return { clamp_index(ix, m_nx), clamp_index(iy, m_ny) };
}

const std::vector<std::size_t>& Grid::candidates(double x, double y) const {
    const auto [ix, iy] = cell_of(x, y);
    return m_cells[cell_index(ix, iy)];
}

BBox2D Grid::bbox() const { 
    return m_bbox; 
}

std::size_t Grid::nx() const { 
    return m_nx; 
}

std::size_t Grid::ny() const { 
    return m_ny; 
}
