#ifndef TRIANGLELOCATOR_HPP
#define TRIANGLELOCATOR_HPP

#include <optional>
#include "mesh2D.hpp"
#include "grid.hpp"

struct TriHit {
    std::size_t triangle_id;
    double a, b, c; // barycentriques
};

class TriangleLocator {
public:
    TriangleLocator(const Mesh2D& mesh, Grid index);

    std::optional<TriHit> locate(double x, double y) const;
    std::optional<double> interpolate(double x, double y) const;

private:
    const Mesh2D& m_mesh;
    Grid m_index;
};

#endif