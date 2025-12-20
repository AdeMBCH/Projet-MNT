#ifndef MESH2D_HPP
#define MESH2D_HPP

#include <vector>
#include <cstddef>
#include <cmath>

struct Vec2 {
    double x = 0.0;
    double y = 0.0;
};

struct BBox2D {
    double minx, miny, maxx, maxy;
};

class Mesh2D {
public:
    Mesh2D(std::vector<double> coords,
           std::vector<std::size_t> triangles,
           std::vector<double> alts);

    std::size_t vertex_count()   const;
    std::size_t triangle_count() const;

    const std::vector<double>& coords() const;
    const std::vector<std::size_t>& triangles() const;
    const std::vector<double>& alts() const;

    Vec2 vertex(std::size_t vi) const;
    void triangle_indices(std::size_t ti, std::size_t& ia, std::size_t& ib, std::size_t& ic) const;

    BBox2D triangle_bbox(std::size_t ti) const;

    bool point_in_triangle(std::size_t ti, const Vec2& p, double eps=1e-12) const;
    bool barycentric(std::size_t ti, const Vec2& p, double& a, double& b, double& c, double eps=1e-18) const;

    double interpolate_z(std::size_t ti, double a, double b, double c) const;

private:
    static double orient2d(const Vec2& a, const Vec2& b, const Vec2& c);

private:
    std::vector<double> m_coords;              // x0,y0,x1,y1,...
    std::vector<std::size_t> m_triangles;      // a0,b0,c0,a1,b1,c1...
    std::vector<double> m_alts;                // z par sommet
};

#endif