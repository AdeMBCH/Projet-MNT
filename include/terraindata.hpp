#ifndef TERRAINDATA_HPP
#define TERRAINDATA_HPP

#include <string>
#include <vector>
#include <limits>


#include "geopoint.hpp"

class TerrainData {
    public:

        TerrainData();

        //Chargement du fichier
        void load_data_from_file(const std::string& filepath);

        //Accès en lecture
        const std::vector<GeoPoint>& points() const;

        std::size_t size() const;

        double min_lat() const;
        double min_lon() const;
        double min_alt() const;
        double max_lat() const;
        double max_lon() const;
        double max_alt() const;

    private:

        void reset_bounds();
        void update_bounds(const GeoPoint& p);

    private:

        std::vector<GeoPoint> m_points; // Vérifier si adresse ou autre
        double m_min_lat;
        double m_min_lon;
        double m_min_alt;
        double m_max_lat;
        double m_max_lon;
        double m_max_alt;


};

#endif