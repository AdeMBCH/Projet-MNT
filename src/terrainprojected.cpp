#include "terrainprojected.hpp"
#include <limits>

TerrainProjected::TerrainProjected(const TerrainData& terrain, const Projector& projector)
{
    m_min_x =  std::numeric_limits<double>::infinity();
    m_max_x = -std::numeric_limits<double>::infinity();
    m_min_y =  std::numeric_limits<double>::infinity();
    m_max_y = -std::numeric_limits<double>::infinity();

    m_pts.reserve(terrain.size());

    for (const auto& p : terrain.points()) {
        Point2D q = projector.project(p.lon, p.lat);

        m_pts.push_back(q);

        if (q.x < m_min_x) m_min_x = q.x;
        if (q.x > m_max_x) m_max_x = q.x;
        if (q.y < m_min_y) m_min_y = q.y;
        if (q.y > m_max_y) m_max_y = q.y;
    }
}

const std::vector<Point2D>& TerrainProjected::points() const { 
    return m_pts; 
}

double TerrainProjected::min_x() const { 
    return m_min_x; 
}

double TerrainProjected::max_x() const { 
    return m_max_x; 
}

double TerrainProjected::min_y() const { 
    return m_min_y; 
}

double TerrainProjected::max_y() const { 
    return m_max_y; 
}
