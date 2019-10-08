/* 
 * File:   mapBin.h
 * Created on February 21, 2019, 12:04 AM
 */

#ifndef MAPBIN_H
#define MAPBIN_H
#include "m1.h"
#include <iostream>
#include "StreetsDatabaseAPI.h"
#include <math.h>
#include <algorithm>
#include <string>
#include "ezgl/point.hpp"
struct groupedBins{
    double x = 0;
    double y = 0;
    std::vector<int> IDs;
};

class MapBin {
public:
    MapBin();
    ~MapBin();
    double m_width, n_width;
    int m, n;
    double max_x, min_x;
    double max_y, min_y;
    double x_length, y_length;
    double latAvg;
    double min_lon, max_lon;
    double min_lat, max_lat;
    int max_ID;
    std::vector<std::vector<int>>                           numberInBin;
    std::vector<std::vector<std::vector<int/*IDs*/>>>       bin;
    
    //Check if it is in the map based on max/min intersection coordinates. 
    bool isInMap(LatLon position);
    bool isInMap(double x, double y);
    bool isInMap(ezgl::point2d p) ;
    bool isInMap(int mIndex, int nIndex);
    //Check if the position is in the given bin.
    bool isInBin(double x,double y,int mIndex,int nIndex);
    bool isInBin(ezgl::point2d p,int mIndex,int nIndex);
    bool isInBin(LatLon position,int mIndex,int nIndex);
    
    //returns the position of the bottom left of bin
    ezgl::point2d binPos(int m, int n);
                                                                                //123    
    //Returns an integer(1-9) representing which side of the bin the position is: 804 returns -1 if not in Bin
    int whereInBin(LatLon my_position, int mIndex,int nIndex);                  //765
    ////Not used or tested.
    
    //Find closest point in the mapBin[m][n] to the position given.
    //Returns pair<distance, intersecionID> if invalid <-1,-1>
    std::pair<double,int> findClosestInBin(LatLon , int mIndex, int nIndex,LatLon (*positionFromID)(int));
    std::pair<double,int> findClosestInBin(double x, double y, int mIndex, int nIndex, LatLon (*positionFromID)(int));
    std::pair<double,int> findClosestInBin(ezgl::point2d p, int mIndex, int nIndex, LatLon (*positionFromID)(int));

    //Finds the closest point to the point using layers of bin around the layer corresponding to the point
    //Returns the intersection ID. If the point given is out of map returns -1.
    int layerFind(LatLon my_position,int numLayers, LatLon (*positionFromID)(int));
    int layerFind(double x,double y,int numLayers, LatLon (*positionFromID)(int));
    int layerFind(ezgl::point2d p,int numLayers, LatLon (*positionFromID)(int));
    
    //Finds the closest point to the point using layers (all layers if necessary) of bin around the layer corresponding to the point
    //Returns the intersection ID. If the point given is out of map returns -1.
    int layerFind(LatLon my_position, LatLon (*positionFromID)(int));
    int layerFind(double x,double y, LatLon (*positionFromID)(int));
    int layerFind(ezgl::point2d p, LatLon (*positionFromID)(int));

    //Find the closest intersection to the point given
    //Returns -1 if the point is out of map or if the distance is not in range.
    int clickFind(LatLon my_position,float range, LatLon (*positionFromID)(int));
    int clickFind(double x,double y,float range, LatLon (*positionFromID)(int));
    int clickFind(ezgl::point2d p,float range, LatLon (*positionFromID)(int));
    
    //Returns the number of layer that would cover the rectangle (-1 if x,y is out of map)
    int layersInRectangle(double x, double y, double width, double height);
    int layersInRectangle(ezgl::point2d center, double width, double height);
    
    //Counts the number of IDs in all the layers
    //Efficient since uses predetermined values
    int countIDsInLayers(double x, double y, int numLayers);
    int countIDsInLayers(ezgl::point2d p, int layers);
    
    //Uses countIDsInLayers to count the IDs in the rectangle given
    //Note the number is showing the IDs in the layers of the bin closely resembling the rectangle and therefore is not 100% accurate
    int countIDsInRectangle(double x, double y, double width, double height);
    int countIDsInRectangle(ezgl::point2d center, double width, double height);
    
    //Adds all intersection around a point (up to the layer specified) to a vector.
    //Limitation: Returns empty vector if given point is out of map.
    std::vector<int> IDsInLayers(double x, double y, int numLayers);
    std::vector<int> IDsInLayers(ezgl::point2d p, int numLayers);
    
    //Receives the center coord., length and width of the screen and returns a vector of points inside.
    //Limitation: currently does not work for when the center is outside the screen
    std::vector<int> IDsInRectangle(double x, double y, double width, double height);
    std::vector<int> IDsInRectangle(ezgl::point2d center, double width, double height);
    
    //Adds all intersection around a point (up to the layer specified) to a vector.
    //Limitation: Returns empty vector if given point is out of map.
    //Returns sorted (ascending) and unique.
    std::vector<int> uniqueIDsInLayers(double x, double y, int numLayers);
    std::vector<int> uniqueIDsInLayers(ezgl::point2d p, int layers);
    
    //Receives the center coord., length and width of the screen and returns a vector of points inside.
    //Limitation: currently does not work for when the center is outside the screen
    //Returns sorted(ascending) and unique.
    std::vector<int> uniqueIDsInRectangle(double x, double y, double width, double height);
    std::vector<int> uniqueIDsInRectangle(ezgl::point2d center, double width, double height);
    
    //Same purpose as IDsInRectangle
    //Can only be used if the rectangle given exactly cover certain number of bins
    //If not returns empty vector with an error message.
    std::vector<int> fineIDsInRectangle(double x, double y, double width, double height);
    
    //Same procedure as fineIDsInRectangle
    //Can only be used if the rectangle given exactly cover certain number of bins
    //If not returns empty vector with an error message.
    groupedBins fineIDsInGroups(double x, double y, double width, double height, LatLon (*positionFromID)(int));
    
    //Groups IDs in a particular range given on the rectangle specified.
    //This function utilizes fineIDsInRectangle to do so.
    //Limitation: the center of the rectangle must not be out of map
    //returns empty if it is.
    std::vector<groupedBins> groupedBinsInRectangle(double x, double y, double width, double height, double range, LatLon (*positionFromID)(int));
    std::vector<groupedBins> groupedBinsInRectangle(ezgl::point2d center, double width, double height, double range, LatLon (*positionFromID)(int));
    
    //Does round down
    int nFromX(double x);
    int mFromY (double y);
    //Clears bin and numberInBin
    void clear();
    private:
    //Find distance between 2 points in a plane
    double distance(double x1, double y1, double x2, double y2);
    double distance(ezgl::point2d p1, ezgl::point2d p2);

    double xFromLon (double lon);
    double yFromLat (double lat);
    ezgl::point2d xyFromLatLon(LatLon position);

    double lonFromX (double x);
    double latFromY (double y);

    LatLon latLonFromXY(ezgl::point2d p);
    LatLon latLonFromXY(double x, double y);
};

#endif /* MAPBIN_H */

