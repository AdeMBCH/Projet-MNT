#include "mesh2D.hpp"
#include <algorithm>

Mesh2D::Mesh2D(std::vector<double> coords, std::vector<std::size_t> triangles, std::vector<double> alts): m_coords(std::move(coords)),m_triangles(std::move(triangles)),m_alts(std::move(alts)){}

Vec2 Mesh2D::vertex(std::size_t vi) const {
    return { m_coords[2 * vi], m_coords[2 * vi + 1] };
}

void Mesh2D::triangle_indices(std::size_t ti, std::size_t& ia, std::size_t& ib, std::size_t& ic) const {
    const std::size_t k = 3 * ti;
    ia = m_triangles[k];
    ib = m_triangles[k + 1];
    ic = m_triangles[k + 2];
}

double Mesh2D::orient2d(const Vec2& a, const Vec2& b, const Vec2& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

BBox2D Mesh2D::triangle_bbox(std::size_t ti) const {
    std::size_t ia, ib, ic;
    triangle_indices(ti, ia, ib, ic);
    const Vec2 A = vertex(ia);
    const Vec2 B = vertex(ib);
    const Vec2 C = vertex(ic);

    BBox2D bb;
    bb.minx = std::min({A.x, B.x, C.x});
    bb.maxx = std::max({A.x, B.x, C.x});
    bb.miny = std::min({A.y, B.y, C.y});
    bb.maxy = std::max({A.y, B.y, C.y});
    return bb;
}

bool Mesh2D::point_in_triangle(std::size_t ti, const Vec2& p, double eps) const {
    std::size_t ia, ib, ic;
    triangle_indices(ti, ia, ib, ic);
    const Vec2 A = vertex(ia);
    const Vec2 B = vertex(ib);
    const Vec2 C = vertex(ic);

    const double w1 = orient2d(A, B, p);
    const double w2 = orient2d(B, C, p);
    const double w3 = orient2d(C, A, p);

    const bool has_neg = (w1 < -eps) || (w2 < -eps) || (w3 < -eps);
    const bool has_pos = (w1 >  eps) || (w2 >  eps) || (w3 >  eps);
    return !(has_neg && has_pos);
}

bool Mesh2D::barycentric(std::size_t ti, const Vec2& p, double& a, double& b, double& c, double eps) const {
    std::size_t ia, ib, ic;
    triangle_indices(ti, ia, ib, ic);
    const Vec2 A = vertex(ia);
    const Vec2 B = vertex(ib);
    const Vec2 C = vertex(ic);

    const double area = orient2d(A, B, C);
    if (std::abs(area) < eps) return false;

    a = orient2d(B, C, p) / area;
    b = orient2d(C, A, p) / area;
    c = 1.0 - a - b;
    return true;
}

double Mesh2D::interpolate_z(std::size_t ti, double a, double b, double c) const {
    std::size_t ia, ib, ic;
    triangle_indices(ti, ia, ib, ic);
    return a * m_alts[ia] + b * m_alts[ib] + c * m_alts[ic];
}

std::size_t Mesh2D::vertex_count()   const { 
    return m_coords.size() / 2; 
}

std::size_t Mesh2D::triangle_count() const { 
    return m_triangles.size() / 3; 
}

const std::vector<double>& Mesh2D::coords() const { 
    return m_coords; 
}

const std::vector<std::size_t>& Mesh2D::triangles() const { 
    return m_triangles; 
}

const std::vector<double>& Mesh2D::alts() const { 
    return m_alts; 
}
