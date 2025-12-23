#include "fourier.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>

FourierPreprocess::FourierPreprocess(Params p) : m_p(p) {}

std::size_t FourierPreprocess::next_pow2(std::size_t v)
{
    std::size_t p = 1;
    while (p < v) p <<= 1;
    return p;
}

void FourierPreprocess::compute_grid_dims(std::size_t target_width,
                                          const BBox2D& bb,
                                          double grid_scale,
                                          bool pow2,
                                          std::size_t& gw,
                                          std::size_t& gh)
{
    const double bw = bb.maxx - bb.minx;
    const double bh = bb.maxy - bb.miny;
    if (bw <= 0.0 || bh <= 0.0) throw std::runtime_error("FourierPreprocess: bbox invalide.");

    const double ratio = bh / bw;

    std::size_t basew = std::max<std::size_t>(16, (std::size_t)std::llround((double)target_width * grid_scale));
    std::size_t baseh = std::max<std::size_t>(16, (std::size_t)std::llround((double)target_width * grid_scale * ratio));

    if (pow2) {
        gw = next_pow2(basew);
        gh = next_pow2(baseh);
    } else {
        gw = basew;
        gh = baseh;
    }
}

static std::vector<double> gaussian_kernel(double sigma)
{
    const int radius = std::max(1, (int)std::ceil(3.0 * sigma));
    const int size = 2 * radius + 1;
    std::vector<double> k(size);

    const double s2 = 2.0 * sigma * sigma;
    double sum = 0.0;
    for (int i = -radius; i <= radius; ++i) {
        const double v = std::exp(-(i * i) / s2);
        k[i + radius] = v;
        sum += v;
    }
    for (double& v : k) v /= sum;
    return k;
}

void FourierPreprocess::bin_average(const std::vector<Point3D>& pts,
                                    const BBox2D& bb,
                                    std::size_t gw, std::size_t gh,
                                    std::vector<double>& z,
                                    std::vector<std::uint8_t>& mask)
{
    z.assign(gw * gh, 0.0);
    mask.assign(gw * gh, 0);

    std::vector<double> sum(gw * gh, 0.0);
    std::vector<std::size_t> cnt(gw * gh, 0);

    const double bw = bb.maxx - bb.minx;
    const double bh = bb.maxy - bb.miny;

    for (const auto& p : pts) {
        const double tx = (p.x - bb.minx) / bw;
        const double ty = (bb.maxy - p.y) / bh; // y inversé
        if (tx < 0.0 || tx >= 1.0 || ty < 0.0 || ty >= 1.0) continue;

        const std::size_t ix = std::min<std::size_t>(gw - 1, (std::size_t)std::floor(tx * (double)gw));
        const std::size_t iy = std::min<std::size_t>(gh - 1, (std::size_t)std::floor(ty * (double)gh));
        const std::size_t id = iy * gw + ix;

        sum[id] += p.z;
        cnt[id] += 1;
    }

    for (std::size_t id = 0; id < gw * gh; ++id) {
        if (cnt[id] > 0) {
            z[id] = sum[id] / (double)cnt[id];
            mask[id] = 1;
        }
    }
}

void FourierPreprocess::fill_missing(std::size_t gw, std::size_t gh,
                                     std::vector<double>& z,
                                     std::vector<std::uint8_t>& mask,
                                     int iters)
{
    if (iters <= 0) return;

    for (int k = 0; k < iters; ++k) {
        auto z2 = z;
        auto m2 = mask;

        for (std::size_t y = 0; y < gh; ++y) {
            for (std::size_t x = 0; x < gw; ++x) {
                const std::size_t id = y * gw + x;
                if (mask[id]) continue;

                double acc = 0.0;
                int n = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        const long xx = (long)x + dx;
                        const long yy = (long)y + dy;
                        if (xx < 0 || yy < 0 || xx >= (long)gw || yy >= (long)gh) continue;
                        const std::size_t jd = (std::size_t)yy * gw + (std::size_t)xx;
                        if (!mask[jd]) continue;
                        acc += z[jd];
                        n++;
                    }
                }
                if (n > 0) {
                    z2[id] = acc / (double)n;
                    m2[id] = 1;
                }
            }
        }

        z.swap(z2);
        mask.swap(m2);
    }
}

void FourierPreprocess::gaussian_separable(std::size_t gw, std::size_t gh,
                                          std::vector<double>& z,
                                          double sigma_px)
{
    if (sigma_px <= 0.0) return;

    auto k = gaussian_kernel(sigma_px);
    const int radius = (int)k.size() / 2;

    std::vector<double> tmp(gw * gh, 0.0);

    // horizontal
    for (std::size_t y = 0; y < gh; ++y) {
        for (std::size_t x = 0; x < gw; ++x) {
            double acc = 0.0;
            for (int dx = -radius; dx <= radius; ++dx) {
                long xx = (long)x + dx;
                if (xx < 0) xx = 0;
                if (xx >= (long)gw) xx = (long)gw - 1;
                acc += k[dx + radius] * z[y * gw + (std::size_t)xx];
            }
            tmp[y * gw + x] = acc;
        }
    }

    // vertical
    for (std::size_t y = 0; y < gh; ++y) {
        for (std::size_t x = 0; x < gw; ++x) {
            double acc = 0.0;
            for (int dy = -radius; dy <= radius; ++dy) {
                long yy = (long)y + dy;
                if (yy < 0) yy = 0;
                if (yy >= (long)gh) yy = (long)gh - 1;
                acc += k[dy + radius] * tmp[(std::size_t)yy * gw + x];
            }
            z[y * gw + x] = acc;
        }
    }
}

std::vector<Point3D> FourierPreprocess::sample_regular(const BBox2D& bb,
                                                       std::size_t gw, std::size_t gh,
                                                       const std::vector<double>& z,
                                                       const std::vector<std::uint8_t>& mask,
                                                       std::size_t step)
{
    if (step == 0) step = 1;

    const double bw = bb.maxx - bb.minx;
    const double bh = bb.maxy - bb.miny;

    const double dx = bw / (double)gw;
    const double dy = bh / (double)gh;

    std::vector<Point3D> out;
    out.reserve((gw / step) * (gh / step));

    for (std::size_t y = 0; y < gh; y += step) {
        const double wy = bb.maxy - (double(y) + 0.5) * dy;
        for (std::size_t x = 0; x < gw; x += step) {
            const std::size_t id = y * gw + x;
            if (!mask[id]) continue;

            const double wx = bb.minx + (double(x) + 0.5) * dx;
            out.push_back({wx, wy, z[id]});
        }
    }
    return out;
}

std::vector<Point3D> FourierPreprocess::run(const std::vector<Point3D>& pts,
                                            const BBox2D& bbox,
                                            std::size_t target_width_px) const
{
    if (target_width_px == 0) throw std::runtime_error("FourierPreprocess: width == 0");

    std::size_t gw = 0, gh = 0;
    compute_grid_dims(target_width_px, bbox, m_p.grid_scale, m_p.pow2_grid, gw, gh);

    const double bw = bbox.maxx - bbox.minx;
    const double bh = bbox.maxy - bbox.miny;

    m_last.gw = gw;
    m_last.gh = gh;
    m_last.dx = bw / (double)gw;
    m_last.dy = bh / (double)gh;

    std::vector<double> z;
    std::vector<std::uint8_t> mask;

    // 1) binning
    bin_average(pts, bbox, gw, gh, z, mask);

    // 2) fill missing (optionnel)
    fill_missing(gw, gh, z, mask, m_p.fill_iters);

    // 3) low-pass (convolution gaussienne) = équivalent à un passe-bas
    gaussian_separable(gw, gh, z, m_p.sigma_px);

    // 4) sample -> points pour Delaunay
    return sample_regular(bbox, gw, gh, z, mask, m_p.sample_step);
}
