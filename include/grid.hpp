#ifndef GRID_HPP
#define GRID_HPP
#include <vector>
#include <cstddef>
#include <optional>
#include "mesh2D.hpp"

class Grid {
    public:
        Grid(const Mesh2D& mesh, BBox2D bbox, std::size_t nx, std::size_t ny);

        // Liste de triangles candidats pour un point p
        const std::vector<std::size_t>& candidates(double x, double y) const;

        BBox2D bbox() const;
        std::size_t nx() const;
        std::size_t ny() const;

    private:

        std::size_t cell_index(std::size_t ix, std::size_t iy) const;
        std::size_t clamp_index(long v, std::size_t maxv) const;

        std::pair<std::size_t,std::size_t> cell_of(double x, double y) const;
    
    private:

        const Mesh2D& m_mesh;
        BBox2D m_bbox;
        std::size_t m_nx;
        std::size_t m_ny;
        double m_dx;
        double m_dy;

        std::vector<std::vector<std::size_t>> m_cells; // pour chaque cellule : triangles
        std::vector<std::size_t> m_empty;              // retourne référence stable si vide
};

#endif
