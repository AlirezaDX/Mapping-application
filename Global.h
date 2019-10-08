#ifndef GLOBAL_H
#define GLOBAL_H

#include "m1.h"
#include <iostream>
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "OSMWay.h"
#include <unordered_set>
#include <set>
#include <map>
#include <math.h>
#include <algorithm>
#include <string>
#include "mapBin.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <curl/curl.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;
using boost::property_tree::ptree;
using boost::property_tree::read_json;

namespace bg = boost::geometry;

//Constants:
constexpr double KM_H_TO_M_S = 1000.0/3600.0;
constexpr int DICTIONARY_SIZE = ('z'-'a'+1) +1; //for Null

const int BIN_INTERSECTION_M_WIDTH = 500; //bins the map into M * N vector (in meters)
const int BIN_INTERSECTION_N_WIDTH = 500; //Optimization tip: m/n should be proportional to map displays height/width

const int BIN_PRIMEINTER_M_WIDTH = 800; //bins the map into M * N vector (in meters)
const int BIN_PRIMEINTER_N_WIDTH = 800; //Optimization tip: m/n should be proportional to map displays height/width


const int BIN_POI_M_WIDTH = 300; //bins the map into M * N vector (in meters)
const int BIN_POI_N_WIDTH = 300; //Optimization tip: m/n should be proportional to map displays height/width

const int BIN_FEATURE_M_WIDTH = 200;//200; //bins the map into M * N vector (in meters)
const int BIN_FEATURE_N_WIDTH = 200;//200; //Optimization tip: m/n should be proportional to map displays height/width

const int BIN_FEATURE_ZOOMED_M_WIDTH = 1300; //bins the map into M * N vector (in meters)
const int BIN_FEATURE_ZOOMED_N_WIDTH = 1300; //Optimization tip: m/n should be proportional to map displays height/width
//Convert character into index of vector


struct dHS{ //Dictionary Helper Structure
        int id;
        std::string prefix;//Up to 3 characters
        std::string leftToCheck;//String holding the rest of the characters
};


struct streetSegmentsTypes{
    std::vector<unsigned> primaryStreets;
    std::vector<unsigned> secondaryStreets;
    std::vector<unsigned> tertiaryStreets;
};

typedef struct dataStruct {
    char *url = nullptr;
    unsigned int size = 0;
    char *response = nullptr;
} dataStruct;

int charToIndex(char c);

class Global{
public:
    
    string programDirectory; //Holds eg. "ece297/work/mapper
    std::vector <string> maps_txtDirectory; //Holds eg. "ece297/work/mapper/libstreetmap/maps.txt 
    string loadedMap; //Holds eg. toronto_canada
    string cityName;  //holds just the city's name eg. toronto
    

    double min_lon, max_lon;
    double min_lat, max_lat;
    double max_x, min_x;
    double max_y, min_y;
    double x_length, y_length;
    double latAvg;
    float maxSpeedLimit = 0;
    //Adjacency list to hold street segments connected to each intersection
    //NOTE: vector [intersection id][street segment]
    
    //Holds all street segments connected to an intersection
    std::vector<std::vector<unsigned/*StreetSegmentIndex*/>>        intersectionStreetSegment;
    
    //Separates street segments connected to an intersection depending on street type
    std::vector<streetSegmentsTypes>                                intersectionStreetStruct;

    //Adjacency list of all street segments that belong to a street
    std::vector<std::vector<unsigned/*StreetSegmentIndex*/>>        streetStreetSegment;

    //Adjacency list containing which intersections an intersection can travel to 
    std::vector<std::unordered_set<unsigned/*IntersectionIndex*/>>  intersectionIntersection;

    //A vector that hold all the information for a street segment
    std::vector<InfoStreetSegment>                                  streetSegmentInfo;

    //A vector holding the time(s) required to travel through a street segment
    std::vector<double>                                             streetSegmentTravelTime;

    //Vectors representing streets that each hold sets containing intersections of each street. 
    //(Note: intersections are sorted therefore find() is log(n))
    std::vector<std::set<unsigned/*IntersectionIndex*/>>            streetIntersection;

    std::vector<std::vector<std::multimap<std::string, unsigned>>>  streetNameDictionary = 
            std::vector<std::vector<std::multimap<std::string, unsigned>>>
            (DICTIONARY_SIZE, std::vector<std::multimap<std::string, unsigned>>(DICTIONARY_SIZE));
    
    //MapBin object to spatially map intersections
    MapBin                                                          intersectionMap;
    
    //MapBin object to spatially map primary intersections
    MapBin                                                          primaryInterMap;
    
    //MapBin object to spatially map POIs
    MapBin                                                          poiMap;
    
    //Because of the certain characteristics of this function MapBin functions that would take (positionFromID) function as an argument will not work for now.
    MapBin                                                          featureMap;
    
    //When the user zooms so that the boundaries of the features is not shown featureMap does not include it
    MapBin                                                          featureZoomedMap;
    
    //Vector of intersections connected to primary roads
    std::vector<IntersectionIndex>                                  mainIntersections;
     
    //Multi-map to hold the tags acquired from the OSM
    std::multimap<OSMID,std::pair<std::string, std::string>>        streetOSMData;
    
    //Vector of feature IDs sorted by area ascending
    std::vector<int>                                                featIDsAreaSorted; 
    int                                                             featIDCloseBegin;
    
    //first string = name, second string = operator name, LatLon = position
    std::vector<std::pair<std::pair<std::string, std::string>, LatLon>>         subwayStationOSMData;
    
    std::vector<std::pair<std::string, LatLon>>                     hotelsOSMData;
    
    std::map<std::string, std::pair<std::string, std::string>>      cityLatLonData;
    
    std::vector<std::string>                                        weatherData;
    
    void loadStructures();
    void clearStructures();
private:
    void loadStreetStructures();
    void loadIntersectionStructures();
    void loadIntersectionMap();
    void loadPrimaryInterMap();
    void loadPOIMap();
    void loadFeaturesArea();
    void loadFeatureMap();
    void loadFeatureZoomedMap();
    void loadStreetNameDictionary();
    void loadStreetOSMData();
    void loadRailwayOSMData();
    void loadHotelsOSMData();
    void loadWeatherData();
    void loadCityLatLonData();
    void getWeatherData(ptree &ptRoot);
    void clearDictionary();
    
    double xFromLon (double lon){
        return EARTH_RADIUS_IN_METERS * DEG_TO_RAD * lon * cos(DEG_TO_RAD * latAvg);
    }
    double yFromLat (double lat){
        return EARTH_RADIUS_IN_METERS * DEG_TO_RAD * lat;
    }
    double lonFromX (double x){
        return x/(DEG_TO_RAD * cos(DEG_TO_RAD * latAvg)) / EARTH_RADIUS_IN_METERS;
    }
    double latFromY (double y){
        return y/ DEG_TO_RAD / EARTH_RADIUS_IN_METERS;
    }  
    
    bool isInMap(LatLon position){
        return (position.lon() <= max_lon && position.lat() <= max_lat && position.lat() >=min_lat && position.lon() >= min_lon);
    }
    bool isInMap(double x, double y){
        return (x <= max_x && y <= max_y && y>= min_y && x>= min_x);
    }
    bool isInMap(ezgl::point2d p){
        return isInMap(p.x, p.y);
    }
};

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

#endif /* GLOBAL_H */

