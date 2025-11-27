#include "projector.hpp"
#include <iostream>

Projector::Projector(const std::string& src_crs, const std::string& dst_crs)
{
    PJ_CONTEXT* C = PJ_DEFAULT_CTX;

    P = proj_create_crs_to_crs(C,src_crs.c_str(),dst_crs.c_str(),nullptr);

    if (!P)
        throw std::runtime_error("Erreur PROJ : impossible d'initialiser la projection.");

    // PROJ prend des radian en entrées pour les coordonnées géo
    P = proj_normalize_for_visualization(C, P);

    if (!P)
        throw std::runtime_error("Erreur PROJ : échec normalisation.");
}

Projector::~Projector()
{
    if (P)
        proj_destroy(P);
}

Point2D Projector::project(double lon_deg, double lat_deg) const
{
    PJ_COORD in;
    in.lpzt.phi = lat_deg;
    in.lpzt.lam = lon_deg;
    in.lpzt.z   = 0;
    in.lpzt.t   = 0;

    PJ_COORD out = proj_trans(P, PJ_FWD, in);

    return { out.xy.x, out.xy.y };
}
