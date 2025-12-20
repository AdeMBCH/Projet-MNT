#pragma once
#include <vector>
#include <cstddef>

class Ombrage {
public:
    // azimuth_deg: 0=N, 90=E ; altitude_deg: hauteur du soleil
    // dx, dy: taille du pixel en "mètres monde"
    static std::vector<double> compute(const std::vector<double>& z,std::size_t w, std::size_t h,double dx, double dy,double azimuth_deg = 315.0,double altitude_deg = 45.0);

private:
    static double deg2rad(double d);
};
