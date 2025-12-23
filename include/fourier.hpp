#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>
#include "mesh2D.hpp"

struct Point3D {
    double x, y, z;
};

class FourierPreprocess {
public:
    struct Params {
        double grid_scale;
        int fill_iters;
        double sigma_px;
        std::size_t sample_step;
        bool pow2_grid;

        Params()
            : grid_scale(1.0),
            fill_iters(4),
            sigma_px(2.0),
            sample_step(2),
            pow2_grid(true)
        {}
    };
    explicit FourierPreprocess(Params p = Params());

    // Entrée: points projetés (x,y,z) + bbox projetée + largeur demandée par l'utilisateur
    // Sortie: points (x,y,z) filtrés + sous-échantillonnés (moins nombreux) pour Delaunay
    std::vector<Point3D> run(const std::vector<Point3D>& pts,
                             const BBox2D& bbox,
                             std::size_t target_width_px) const;

    // Juste info/debug : dimensions de grille utilisées
    struct GridInfo {
        std::size_t gw = 0, gh = 0;
        double dx = 0.0, dy = 0.0;
    };

    GridInfo last_grid() const { return m_last; }

private:
    Params m_p;
    mutable GridInfo m_last;

private:
    static std::size_t next_pow2(std::size_t v);

    static void compute_grid_dims(std::size_t target_width,
                                  const BBox2D& bb,
                                  double grid_scale,
                                  bool pow2,
                                  std::size_t& gw,
                                  std::size_t& gh);

    static void bin_average(const std::vector<Point3D>& pts,
                            const BBox2D& bb,
                            std::size_t gw, std::size_t gh,
                            std::vector<double>& z,
                            std::vector<std::uint8_t>& mask);

    static void fill_missing(std::size_t gw, std::size_t gh,
                             std::vector<double>& z,
                             std::vector<std::uint8_t>& mask,
                             int iters);

    static void gaussian_separable(std::size_t gw, std::size_t gh,
                                   std::vector<double>& z,
                                   double sigma_px);

    static std::vector<Point3D> sample_regular(const BBox2D& bb,
                                               std::size_t gw, std::size_t gh,
                                               const std::vector<double>& z,
                                               const std::vector<std::uint8_t>& mask,
                                               std::size_t step);
};
