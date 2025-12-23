#include "colormap.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>

static RGB lerp(RGB a, RGB b, double u) {
    u = std::clamp(u, 0.0, 1.0);
    auto L = [&](std::uint8_t x, std::uint8_t y) -> std::uint8_t {
        return static_cast<std::uint8_t>(std::lround((1.0 - u) * x + u * y));
    };
    return { L(a.r,b.r), L(a.g,b.g), L(a.b,b.b) };
}

static bool parse_rgb_slash(const std::string& s, RGB& out) {
    int r,g,b;
    char c1, c2;
    std::istringstream iss(s);
    if (!(iss >> r >> c1 >> g >> c2 >> b)) return false;
    if (c1 != '/' || c2 != '/') return false;
    if (r<0||r>255||g<0||g>255||b<0||b>255) return false;
    out = { (std::uint8_t)r, (std::uint8_t)g, (std::uint8_t)b };
    return true;
}

void HaxbyColorMap::load_cpt(const std::string& filepath)
{
    std::ifstream f(filepath);
    if (!f) throw std::runtime_error("Impossible d'ouvrir la palette: " + filepath);

    m_segments.clear();

    // Format ligne:
    // t0   r0/g0/b0   t1   r1/g1/b1
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        std::istringstream iss(line);
        double t0, t1;
        std::string rgb0, rgb1;

        if (!(iss >> t0 >> rgb0 >> t1 >> rgb1)) continue;

        RGB c0, c1;
        if (!parse_rgb_slash(rgb0, c0)) continue;
        if (!parse_rgb_slash(rgb1, c1)) continue;

        m_segments.push_back({t0, t1, c0, c1});
    }

    if (m_segments.empty())
        throw std::runtime_error("Palette vide/illisible: " + filepath);

    std::sort(m_segments.begin(), m_segments.end(),
              [](const Segment& a, const Segment& b){ return a.t0 < b.t0; });

    build_lut_256();
    m_ready = true;
}

void HaxbyColorMap::build_lut_256()
{
    for (int i = 0; i < 256; ++i) {
        const double t = (double)i / 255.0;

        // Trouver le segment contenant t
        const Segment* seg = nullptr;
        for (const auto& s : m_segments) {
            if (t >= s.t0 && t <= s.t1) { seg = &s; break; }
        }
        if (!seg) {
            // clamp si hors bornes
            if (t < m_segments.front().t0) m_lut[(std::size_t)i] = m_segments.front().c0;
            else                           m_lut[(std::size_t)i] = m_segments.back().c1;
            continue;
        }

        const double denom = (seg->t1 - seg->t0);
        const double u = (denom <= 0.0) ? 0.0 : (t - seg->t0) / denom;
        m_lut[(std::size_t)i] = lerp(seg->c0, seg->c1, u);
    }
}

RGB HaxbyColorMap::color(double z, double zmin, double zmax) const
{
    if (!m_ready) return {0,0,0};
    if (zmax <= zmin) return m_lut[0];

    double t = (z - zmin) / (zmax - zmin);
    t = std::clamp(t, 0.0, 1.0);

    const int idx = (int)std::lround(t * 255.0);
    return m_lut[(std::size_t)idx];
}

RGB HaxbyColorMap::shade(RGB c, double s)
{
    s = std::clamp(s, 0.0, 1.0);
    auto mul = [&](std::uint8_t v) -> std::uint8_t {
        return static_cast<std::uint8_t>(std::lround((double)v * s));
    };
    return { mul(c.r), mul(c.g), mul(c.b) };
}
