#ifndef GEOPOINT_HPP
#define GEOPOINT_HPP

#include <ostream>

struct GeoPoint {
    double lat;
    double lon;
    double alt;

    GeoPoint(): lat(0.0),lon(0.0),alt(0.0){}
    GeoPoint(double lat_, double lon_, double alt_): lat(lat_),lon(lon_),alt(alt_){}

};

inline std::ostream& operator<<(std::ostream& os, const GeoPoint& p){
    os << p.lat << " " << p.lon << " " << p.alt;
    return os;
}

#endif