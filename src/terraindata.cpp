#include <limits>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "terraindata.hpp"


TerrainData::TerrainData(){
    reset_bounds();
}

void TerrainData::load_data_from_file(const std::string& filepath)
{
    std::ifstream ifs(filepath);
    if (!ifs) {
        throw std::runtime_error("Impossible d'ouvrir le fichier MNT : " + filepath);
    }

    m_points.clear();
    reset_bounds();

    std::string line;
    std::size_t line_number = 0;

    while (std::getline(ifs, line)) {
        ++line_number;

        // Ignorer les lignes vides ou purement blanches
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        double lat, lon, alt;

        // Format attendu : lat lon alt
        if (!(iss >> lat >> lon >> alt)) {
            // Ligne mal formée : on peut soit ignorer, soit lever une exception.
            // Ici on lève, pour forcer un fichier propre.
            throw std::runtime_error(
                "Ligne " + std::to_string(line_number) +
                " mal formée dans le fichier MNT."
            );
        }

        GeoPoint p(lat, lon, alt);
        m_points.push_back(p);
        update_bounds(p);
    }

    if (m_points.empty()) {
        throw std::runtime_error("Fichier MNT vide ou sans données valides : " + filepath);
    }
}

const std::vector<GeoPoint>& TerrainData::points() const{
    return m_points;
}


void TerrainData::reset_bounds(){
    m_min_lat = std::numeric_limits<double>::infinity();
    m_min_lon = std::numeric_limits<double>::infinity();
    m_min_alt = std::numeric_limits<double>::infinity();
    m_max_lat = -std::numeric_limits<double>::infinity();
    m_max_lon = -std::numeric_limits<double>::infinity();
    m_max_alt = -std::numeric_limits<double>::infinity();
}

void TerrainData::update_bounds(const GeoPoint& p){
    if (p.lat < m_min_lat) m_min_lat = p.lat;
    if (p.lon < m_min_lon) m_min_lon = p.lon;
    if (p.alt < m_min_alt) m_min_alt = p.alt;
    if (p.lat > m_max_lat) m_max_lat = p.lat;
    if (p.lon > m_max_lon) m_max_lon = p.lon;
    if (p.alt > m_max_alt) m_max_alt = p.alt;
}

std::size_t TerrainData::size() const{
    return m_points.size();
}

double TerrainData::min_lat() const{
    return m_min_lat;
}

double TerrainData::min_lon() const{
    return m_min_lon;
}

double TerrainData::min_alt() const{
    return m_min_alt;
}

double TerrainData::max_lat() const{
    return m_max_lat;
}

double TerrainData::max_lon() const{
    return m_max_lon;
}

double TerrainData::max_alt() const{
    return m_max_alt;
}