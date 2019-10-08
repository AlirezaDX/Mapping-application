/* 
 * File:   m1Helper.h
 * Created on February 17, 2019, 3:08 PM
 */

#ifndef M1HELPER_H
#define M1HELPER_H
#include "m1.h"
#include "Global.h"
#include <iostream>
#include "StreetsDatabaseAPI.h"
#include <math.h>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include "ezgl/point.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/rectangle.hpp"
#include <cstdlib> //To use free/malloc used for readline
#include <readline/readline.h>

#include <vector>
#include <string>

const std::string MAPS_DIRECTORY = "/cad2/ece297s/public/maps/";

using namespace std;

extern Global _DATA;
extern double ZOOM_BOUNDARY_1;
extern double ZOOM_BOUNDARY_2;
extern double ZOOM_BOUNDARY_3;
extern double ZOOM_BOUNDARY_4;
extern double ZOOM_BOUNDARY_5;

//Find distance between 2 points in a plane
double distance(double x1, double y1, double x2, double y2);
double distance(ezgl::point2d p1, ezgl::point2d p2);

//Check inclusively if x is between y1 , y2
//Unused.
bool isBetween(const double x, const double y1, const double y2);

double xFromLon (double lon);
double yFromLat (double lat);
ezgl::point2d xyFromLatLon(LatLon);

double lonFromX (double x);
double latFromY (double y);
LatLon latLonFromXY(ezgl::point2d p);
LatLon latLonFromXY(double x, double y);

//Check if it is in the map based on max/min intersection coordinates.
bool isInMap(LatLon position);
bool isInMap(double x,double y);
bool isInMap(ezgl::point2d p);

//Checks the position of the point with respect to the rectangle
bool isOnLeft(const ezgl::rectangle&, const ezgl::point2d&);
bool isOnRight(const ezgl::rectangle&, const ezgl::point2d&);
bool isAbove(const ezgl::rectangle&, const ezgl::point2d&);
bool isBelow(const ezgl::rectangle&, const ezgl::point2d&);

 /*                                                                      -1  1 -2
                                                                            ---
                                                                         4  ---  2
                                                                            ---
* Returns the id at which the point is positioned outside the rectangle: -4  3 -3   */
int posIDOutsideRectangle(const ezgl::rectangle& visible,const ezgl::point2d& p);
                                                
/*                                                  1---2                                          
* Return the corner point corresponding to the ID:   ---  
* returns zero if id invalid                        4---3
 */ 
ezgl::point2d cornerPosFromID(const ezgl::rectangle& visible, const int& id);


//Assuming that the 2 points given are both outside of the rectangle and 
//the line connecting them does not intersect with the screen:
//Returns the corner points of the rectangle which is between the projection of those 2 points on the rectangle.
//Note that projection of a point on facing an edge is the edge.
std::vector<ezgl::point2d> passedCorner(const ezgl::rectangle& visible, const ezgl::point2d& prevPoint, const ezgl::point2d& currPoint);

//Takes 2 points one inside and one outside the rectangle and finds the intersection of the line with the rectangle
ezgl::point2d screenIntersection(const ezgl::rectangle& visible, const ezgl::point2d& inside, const ezgl::point2d& outside );

//Takes the screen rectangle and a polynomial points clips those points to fit only in the screen.
//Bug: if the line connecting two consecutive points outside intersects(twice) with the rectangle
//Returns no points if all points are outside the rectangle
std::vector<ezgl::point2d> clipPoly(const ezgl::rectangle&, std::vector<ezgl::point2d>&);
std::vector<ezgl::point2d> clipPolyDebug(ezgl::renderer &g, const ezgl::rectangle&, std::vector<ezgl::point2d>);

//Load the text file holding maps available into a vector of strings
std::vector<std::string> loadMapText(std::string);

//Takes a string and finds the city name in the map that is most similar to it
//Returns a vector of IDs of the cities that have the same number of similarities
//Case sensitive
std::vector<int> getCityIDs(string userInputMap, std::vector<std::string> maps);

//Takes the map ID as input and returns the map path
//eg. <"/cad2/ece297s/public/maps/toronto_canada">
std::string getMapPath(int mapID,std::vector<std::string> maps);

//To parse user input:
//Asks user for a city name uses readline to take it in (<case insensitive>)
//Uses getCityIDs to find the mapIDs that are similar to the input
//If there are more than one map found or no map found asks for another city
//Returns the street map path of the city found using getMapPath
//eg. <"/cad2/ece297s/public/maps/toronto_canada.street.bin">
std::string getStreetMapPath(std::vector<std::string> maps);

//Takes current map area and zoom boundaries of map
//Returns int representing which zoom level is the map currently in
//  5 - ZOOM_BOUNDARY_5
//  4 - ZOOM_BOUNDARY_4
//  3 - ZOOM_BOUNDARY_3
//  2 - ZOOM_BOUNDARY_2 
//  1 - map area greater than all of the above
int calculateZoomLevel(double areaDisplayed);


#endif /* M1HELPER_H */

