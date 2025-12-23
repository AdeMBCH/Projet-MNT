#ifndef COLORMAP_HPP
#define COLORMAP_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

struct RGB { 
    std::uint8_t r, g, b; 
};

class HaxbyColorMap {

    public:

        void load_cpt(const std::string& filepath);   // resources/haxby.cpt

        RGB color(double z, double zmin, double zmax) const;

        static RGB shade(RGB c, double s);

    private:

        struct Segment {
            double t0, t1;   // dans [0,1]
            RGB c0, c1;
        };

        std::vector<Segment> m_segments;
        std::array<RGB,256> m_lut{};
        bool m_ready = false;

        void build_lut_256();
    };

#endif