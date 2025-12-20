#include "trianglelocator.hpp"

TriangleLocator::TriangleLocator(const Mesh2D& mesh, Grid index): m_mesh(mesh), m_index(std::move(index)){}

std::optional<TriHit> TriangleLocator::locate(double x, double y) const {
    const Vec2 p{x, y};
    const auto& cand = m_index.candidates(x, y);

    for (std::size_t ti : cand) {
        if (!m_mesh.point_in_triangle(ti, p)) continue;

        double a, b, c;
        if (!m_mesh.barycentric(ti, p, a, b, c)) continue;

        return TriHit{ti, a, b, c};
    }
    return std::nullopt;
}

std::optional<double> TriangleLocator::interpolate(double x, double y) const {
    auto hit = locate(x, y);
    if (!hit){
        return std::nullopt;
    }
    return m_mesh.interpolate_z(hit->triangle_id, hit->a, hit->b, hit->c);
}
