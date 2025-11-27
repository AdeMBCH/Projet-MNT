#ifndef PROJECTOR_HPP
#define PROJECTOR_HPP

#include <proj.h>
#include <stdexcept>
#include <string>

struct Point2D{
    double x;
    double y;
};

class Projector {
public:
    Projector(
        const std::string& src_crs = "+proj=longlat +datum=WGS84",
        const std::string& dst_crs =
        "+proj=lcc +lat_1=49 +lat_2=44 +lat_0=46.5 +lon_0=3 "
        "+x_0=700000 +y_0=6600000 +ellps=GRS80 +units=m +no_defs"
    );

    ~Projector();

    Point2D project(double lon_deg, double lat_deg) const;

private:
    PJ* P; // pipeline de transformation
};

#endif 