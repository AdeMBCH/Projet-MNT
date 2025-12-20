#ifndef COLORMAP_HPP
#define COLORMAP_HPP

#include <cstdint>
#include <vector>

struct RGB {
    std::uint8_t r, g, b;
};

class ColorMapHaxby {
public:
    ColorMapHaxby(double zmin, double zmax);
    RGB operator()(double z) const;

    // Modulation hillshade [0..1]
    static RGB shade(RGB c, double s);

private:
    struct Stop {
        double t;
        std::uint8_t r, g, b;
    };

    double normalize(double z) const;
    static RGB lerp(const Stop& a, const Stop& b, double t);
    RGB sample(double t) const;

    double m_zmin;
    double m_zmax;
};

#endif
