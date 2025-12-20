#include "colormap.hpp"

#include <algorithm>
#include <cmath> 

ColorMapHaxby::ColorMapHaxby(double zmin, double zmax): m_zmin(zmin), m_zmax(zmax) {}

RGB ColorMapHaxby::operator()(double z) const {
    const double t = normalize(z);
    return sample(t);
}

RGB ColorMapHaxby::shade(RGB c, double s) {
    s = std::clamp(s, 0.0, 1.0);
    auto mul = [&](std::uint8_t v) -> std::uint8_t {
        return static_cast<std::uint8_t>(std::lround(static_cast<double>(v) * s));
    };
    return { mul(c.r), mul(c.g), mul(c.b) };
}

double ColorMapHaxby::normalize(double z) const {
    if (m_zmax <= m_zmin){
        return 0.0;
    }
    double t = (z - m_zmin) / (m_zmax - m_zmin);
    return std::clamp(t, 0.0, 1.0);
}

RGB ColorMapHaxby::lerp(const Stop& a, const Stop& b, double t) {
    const double u = (t - a.t) / (b.t - a.t);
    auto L = [&](std::uint8_t x, std::uint8_t y) -> std::uint8_t {
        return static_cast<std::uint8_t>(
            std::lround((1.0 - u) * x + u * y)
        );
    };
    return { L(a.r, b.r), L(a.g, b.g), L(a.b, b.b) };
}

RGB ColorMapHaxby::sample(double t) const {
    static const std::vector<Stop> lut = {
        {0.00,  10,  20,  70},
        {0.10,  20,  60, 140},
        {0.20,  30, 110, 190},
        {0.30,  70, 170, 220},
        {0.40, 120, 210, 180},
        {0.50, 170, 230, 120},
        {0.60, 220, 220,  80},
        {0.70, 240, 180,  60},
        {0.80, 230, 120,  70},
        {0.90, 210, 170, 170},
        {1.00, 245, 245, 245}
    };

    if (t <= lut.front().t){
        return {lut.front().r, lut.front().g, lut.front().b};
    }
    if (t >= lut.back().t){
        return {lut.back().r,  lut.back().g,  lut.back().b};
    }

    for (std::size_t i = 0; i + 1 < lut.size(); ++i) {
        if (t >= lut[i].t && t <= lut[i + 1].t) {
            return lerp(lut[i], lut[i + 1], t);
        }
    }
    return {0, 0, 0};
}
