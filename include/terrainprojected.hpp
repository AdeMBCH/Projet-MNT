#ifndef TERRAINPROJECTED_H
#define TERRAINPROJECTED_H

#include <vector>
#include "terraindata.hpp"
#include "projector.hpp"

class TerrainProjected {
public:

    TerrainProjected(const TerrainData& terrain, const Projector& projector);

    const std::vector<Point2D>& points() const;

    double min_x() const;
    double max_x() const;
    double min_y() const;
    double max_y() const;

private:

    std::vector<Point2D> m_pts;
    
    double m_min_x;
    double m_max_x;
    double m_min_y;
    double m_max_y;
};

#endif
