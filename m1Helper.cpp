#include "m1Helper.h"

//Global variable used through the program to hold the data structures.
Global _DATA;

double distance(double x1, double y1, double x2, double y2){
    return sqrt(pow((y2-y1),2) + pow((x2-x1),2));
}
double distance(ezgl::point2d p1, ezgl::point2d p2){
    return sqrt(pow((p2.y-p1.y),2) + pow((p2.x-p1.x),2));
}


double xFromLon (double lon){
    return EARTH_RADIUS_IN_METERS * DEG_TO_RAD * lon * cos(DEG_TO_RAD * _DATA.latAvg);
}
double yFromLat (double lat){
    return EARTH_RADIUS_IN_METERS * DEG_TO_RAD * lat;
}
ezgl::point2d xyFromLatLon(LatLon position){
    ezgl::point2d ans(xFromLon(position.lon()), yFromLat(position.lat()));
    return ans;
}

double lonFromX (double x){
    return x/(DEG_TO_RAD * cos(DEG_TO_RAD * _DATA.latAvg)) / EARTH_RADIUS_IN_METERS;
}
double latFromY (double y){
    return y/ DEG_TO_RAD / EARTH_RADIUS_IN_METERS;
}

LatLon latLonFromXY(ezgl::point2d p){
    double lat = latFromY(p.y);
    double lon = lonFromX(p.x);
    LatLon ans(lat,lon);
    return ans;
}
LatLon latLonFromXY(double x, double y){
    double lat = latFromY(y);
    double lon = lonFromX(x);
    LatLon ans(lat,lon);
    return ans;
}

bool isInMap(LatLon position){
    return (position.lon() <= _DATA.max_lon && position.lat() <= _DATA.max_lat && position.lat() >=_DATA.min_lat && position.lon() >=_DATA.min_lon);
}
bool isInMap(double x, double y){
    return (x <= _DATA.max_x && y <= _DATA.max_y && y>= _DATA.min_y && x>= _DATA.min_x);
}
bool isInMap(ezgl::point2d p){
    return isInMap(p.x, p.y);
}

std::vector<std::string> loadMapText(std::string directory_path){
    ifstream mapStream; //input file <fstream>
    string completePath = directory_path + "/libstreetmap/maps.txt";
    mapStream.open( completePath ); //In the same directory
    //cout << completePath  << endl;
    
    std::vector<std::string> maps;
    string map;
    while(1==1){//Pushing back all of the maps in maps.txt into a vector of strings
        mapStream >> map;
        if(!mapStream.fail())
            maps.push_back( map );
        else break;
    }
    
    mapStream.close();
    return maps;
}

//Case sensitive
//Return city IDs that the city name matches the most character of the input (minimum 3 characters.)
std::vector<int> getCityIDs(string userInputMap, std::vector<std::string> maps){
    int bestNumSimilarity=0;
    
    std::vector<int> citiesMatching;
    
    for(unsigned i=0; i< maps.size(); i++){ //Go through vector of maps available and find number of similarities between the city name in it with the city name provided
        
        int compNumSimilarity=0; //Competitors number of similarities.
        string cityName = maps[i].substr(0, maps[i].find_last_of("_"));//To isolate the city's name
        
        for(unsigned j=0; j<std::min(userInputMap.length(),maps[i].length()); j++){//Find the number of similar characters between the two    
            if(userInputMap[j]==maps[i][j])//Check for similarity
                compNumSimilarity ++;
        }
        if(compNumSimilarity == bestNumSimilarity && compNumSimilarity>2){//Check if competitor is as similar as previous winner
            citiesMatching.push_back(i);
        }
        if(compNumSimilarity > bestNumSimilarity && compNumSimilarity>2){//Check if competitor wins and becomes the new winner
            citiesMatching.clear();
            citiesMatching.push_back(i);
            bestNumSimilarity = compNumSimilarity;
        }
    }
    return citiesMatching;
}

std::string getMapPath(int mapID, std::vector<std::string> maps){
    return MAPS_DIRECTORY + maps[mapID];
}

int calculateZoomLevel(double areaDisplayed){
    if(areaDisplayed < ZOOM_BOUNDARY_5)
        return 5;
    else if(areaDisplayed < ZOOM_BOUNDARY_4)
        return 4;
    else if(areaDisplayed < ZOOM_BOUNDARY_3)
        return 3;
    else if(areaDisplayed < ZOOM_BOUNDARY_2)
        return 2;
    else
        return 1;
}

//Case insensitive
std::string getStreetMapPath(std::vector<std::string> maps){ // <=-=> WIP <=-=>
    char* buf;
    while( (buf = readline("City to load> ")) != NULL ){ //Basically infinite loop//Use readline to put input in buf
        std::string buffer(buf); //String constructor for char*.
        //Transform user input to lower case.
        transform(buffer.begin(), buffer.end(), buffer.begin(),::tolower);
        
        if(buffer=="quit" || buffer=="end" || buffer=="")
            return "end"; //End program in main
        
        vector<int> citiesMatchingIDs = getCityIDs(buffer,maps);
        
        if(citiesMatchingIDs.size()>1){ // <=-=> Multiple cities matched users input. <=-=>
            
        }
        else if(citiesMatchingIDs.size()>0){ //If the function just returned one map ID as the most similar:
            std::string mapPath = getMapPath(citiesMatchingIDs[0], maps);
            //cout << mapPath << endl;
            return mapPath + ".streets.bin"; //Returns streetMapPath for exerciser.
        }
        free(buf);
        buf = NULL;
    }
    return "";
}


bool isOnLeft(const ezgl::rectangle& visible, const ezgl::point2d& ptOutside){
    if(ptOutside.x < visible.left())
        return true;
    return false;
}

bool isOnRight(const ezgl::rectangle& visible, const ezgl::point2d& ptOutside){
    if(ptOutside.x > visible.right())
        return true;
    return false;
}

bool isAbove(const ezgl::rectangle& visible, const ezgl::point2d& ptOutside){
    if(ptOutside.y > visible.top())
        return true;
    return false;
}

bool isBelow(const ezgl::rectangle& visible, const ezgl::point2d& ptOutside){
    if(ptOutside.y < visible.bottom())
        return true;
    return false;
}

bool isBetween(const double x, const double y1, const double y2){
    if( (y1<=x && x<=y2) || (y2<=x && x<=y1) )
        return true;
    return false;
}

/*Functions for polyclipping*/
int posIDOutsideRectangle(const ezgl::rectangle& visible,const ezgl::point2d& p){
    bool pIsAbove      = isAbove(visible, p);
    bool pIsRight      = isOnRight(visible, p);
    bool pIsBelow      = isBelow(visible, p);
    bool pIsLeft       = isOnLeft(visible, p);

    if(pIsAbove){
        if(pIsLeft)
            return -1;
        if(pIsRight)
            return -2;
        return 1;
    }
    if(pIsRight){
        if(pIsAbove)
            return -2;
        if(pIsBelow)
            return -3;
        return 2;                   
    }
    if(pIsBelow){
        if(pIsRight)
            return -3;
        if(pIsLeft)
            return -4;
        return 3;
    }
    if(pIsLeft){//USeless
        if(pIsBelow)
            return -4;
        if(pIsAbove)
            return -1;
        return 4;
    }
    std::cout << "BIG ERROR: point inside.";
    return 0;
}

ezgl::point2d cornerPosFromID(const ezgl::rectangle& visible, const int& id){
    switch(id){
        case 1:{
            return visible.top_left();
        }
        case 2:{
            return visible.top_right();
        }
        case 3:{
            return visible.bottom_right();
        }
        case 4:{
            return visible.bottom_left();
        }
        default:{
            std::cout<< "Small error";
            ezgl::point2d ans(0,0);
            return ans;
        }
    }
            
}

std::vector<ezgl::point2d> passedCorner(const ezgl::rectangle& visible, const ezgl::point2d& prevPoint, const ezgl::point2d& currPoint){
    
    int prevPointPosID = posIDOutsideRectangle(visible, prevPoint);
    int currPointPosID = posIDOutsideRectangle(visible, currPoint);
    
    if(prevPointPosID == currPointPosID){
        std::vector<ezgl::point2d> empty;
        return empty;
    }
    
    std::vector<ezgl::point2d> ans;
    bool isPrevPosCorner =false;
    if(prevPointPosID<0){
        isPrevPosCorner = true;
        prevPointPosID = -prevPointPosID;
        int& cornerID = prevPointPosID;
        ans.push_back(cornerPosFromID(visible, cornerID));
    }
    bool isCurrPosCorner =false;
    if(currPointPosID<0){
        isCurrPosCorner = true;
        currPointPosID = -currPointPosID;
    }
    
    switch(prevPointPosID){
        case 1:{
            switch(currPointPosID){
                case 1:{
                    break;
                }
                case 2:{
                    if(!isCurrPosCorner)
                        ans.push_back(cornerPosFromID(visible, 2));
                    break;
                }
                case 3:{
                    //if(!ans.empty() && ans.back() != cornerPosFromID(visible, 1) || ans.empty())
                    //   ans.push_back(cornerPosFromID(visible, 1));
                    //^^ Difenitley included already
                    ans.push_back(cornerPosFromID(visible, 4));
                    break;
                }
                case 4:{
                    if(!isPrevPosCorner)
                        ans.push_back(cornerPosFromID(visible, 1));
                    break;
                }
                default:{
                    std::cout<<"Major error 1";
                    break;
                }
            }
            break;
        }
        case 2:{
            switch(currPointPosID){
                case 1:{
                    if(!isPrevPosCorner)
                        ans.push_back(cornerPosFromID(visible, 2));
                    break;
                }
                case 2:{
                    break;
                }
                case 3:{
                    if(!isCurrPosCorner)
                        ans.push_back(cornerPosFromID(visible, 3));
                    break;
                }
                case 4:{
                    //if(!ans.empty() && ans.back() != cornerPosFromID(visible, 2) || ans.empty())
                    //   ans.push_back(cornerPosFromID(visible, 2));
                    //^^ Difenitley included already
                    ans.push_back(cornerPosFromID(visible, 1));
                    break;
                }
                default:{
                    std::cout<<"Major error 2";
                    break;
                }
            }
            break;
        }
        case 3:{
            switch(currPointPosID){
                case 1:{
                    //if(!ans.empty() && ans.back() != cornerPosFromID(visible, 3) || ans.empty())
                    //   ans.push_back(cornerPosFromID(visible, 3));
                    //^^ Difenitley included already
                    ans.push_back(cornerPosFromID(visible, 2));
                    break;
                }
                case 2:{
                    if(!isPrevPosCorner)
                        ans.push_back(cornerPosFromID(visible, 3));
                    break;
                }
                case 3:{
                    break;
                }
                case 4:{
                    if(!isCurrPosCorner)
                        ans.push_back(cornerPosFromID(visible, 4));
                    break;
                }
                default:{
                    std::cout<<"Major error 3";
                    break;
                }
            }
            break;
        }
        case 4:{
            switch(currPointPosID){
                case 1:{
                    if(!isCurrPosCorner)
                        ans.push_back(cornerPosFromID(visible, 1));
                    break;
                }
                case 2:{
                    //if(!ans.empty() && ans.back() != cornerPosFromID(visible, 4) || ans.empty())
                    //   ans.push_back(cornerPosFromID(visible, 4));
                    //^^ Difenitley included already
                    ans.push_back(cornerPosFromID(visible, 3));
                    break;
                }
                case 3:{
                    if(!isPrevPosCorner)
                        ans.push_back(cornerPosFromID(visible, 4));
                    break;
                }
                case 4:{
                    break;
                }
                default:{
                    std::cout<<"Major error 4";
                    break;
                }
            }
            break;
        }
        default:{
            std::cout<<"Major error M";
            break;
        }
    }
    
    if(isCurrPosCorner){
        int& cornerID = currPointPosID;
        //if(!ans.empty() && ans.back() != cornerPosFromID(visible, cornerID) || ans.empty())
        ans.push_back(cornerPosFromID(visible, cornerID));
    }
    return ans;
}

//Assumes the points are passed correctly inside and outside.
ezgl::point2d screenIntersection(const ezgl::rectangle& visible, const ezgl::point2d& pointInside, const ezgl::point2d& pointOutside ){
    double dY = pointOutside.y - pointInside.y;
    double dX = pointOutside.x - pointInside.x;
    if(dY==0) dY=0.000000000000000000000000000000000000000000001;
    if(dX==0) dX=0.000000000000000000000000000000000000000000001;
    if(dX!= 0){
        if(dX>0){ //Going right
            double possibleInterX;
            double possibleInterY = ((visible.right() - pointInside.x) * dY/dX) + pointInside.y;
            if(dY>0){
                possibleInterX = ((visible.top() - pointInside.y) * dX/dY) + pointInside.x;
            }
            else{
                possibleInterX = ((visible.bottom() - pointInside.y) * dX/dY) + pointInside.x;
            }
            //We found possibleInterX/Y
            if(possibleInterX < visible.right()){ //If hitting the upper or lower border
                if(dY<0){
                    return{possibleInterX, visible.bottom()};
                }
                else{
                    return {possibleInterX, visible.top()};
                }
            }
            else{ //If hitting the right border
                return {visible.right(), possibleInterY};
            }
        }
        else{ //Going left
            double possibleInterX;
            double possibleInterY = ((visible.left() - pointInside.x) * dY/dX) + pointInside.y;
            if(dY>0){ //Going up
                possibleInterX = ((visible.top() - pointInside.y) * dX/dY) + pointInside.x;
            }
            else{ //Or down
                possibleInterX = ((visible.bottom() - pointInside.y) * dX/dY) + pointInside.x;
            }
            if(possibleInterX > visible.left()){ //If hitting the upper or lower border
                if(dY<0){
                    return {possibleInterX, visible.bottom()};
                }
                else{
                    return {possibleInterX, visible.top()};
                }
            }
            else{ //If hitting the left border
                return {visible.left(), possibleInterY};
            }
        }
    }
    else{ //Moving vertically(dX==0)
        if(dY>0)
            return {pointInside.x, visible.top()};
        else
            return {pointInside.x, visible.bottom()};
    }
}

std::vector<ezgl::point2d> clipPoly(const ezgl::rectangle& visible, std::vector<ezgl::point2d>& polyShape)
{
    if(polyShape.size()<2){
        std::vector<ezgl::point2d> empty;
        return empty;
    }
    polyShape.pop_back(); //Kick out the last element in the vector
    //We will push it back before returning
    
    std::vector<ezgl::point2d> clippedPoly;
    //std::vector<unsigned> clippedPointIndex;
    
    int startInterIndex = -1;
    
    bool pointOutOfBounds = false; //By default we assume first point is in.
    
    for(unsigned i=0; i<polyShape.size(); i++){ //Find first point in rectangle
        if(visible.contains(polyShape[i].x, polyShape[i].y)){
            startInterIndex = i;
            break;
        }
    }
    if(startInterIndex == -1){ //All points were out of screen so we should draw the rectangle
        std::vector<ezgl::point2d> screenShape;
        /*screenShape.push_back(visible.top_left());
        screenShape.push_back(visible.bottom_left());
        screenShape.push_back(visible.bottom_right());
        screenShape.push_back(visible.top_right());
        screenShape.push_back(visible.top_left());*/
        polyShape.push_back(polyShape.front()); //We kicked it out at first putting it back before returning
        return screenShape;
    }
    
    int polyShapeIndex = startInterIndex;
    for(unsigned i=0; i<=polyShape.size(); i++,polyShapeIndex++){ //We start with first in screen and end with it. (<=)
        if(polyShapeIndex==(int)polyShape.size())
            polyShapeIndex=0; //Make the index start at first point inside the screen and to wrap around
        
        int prevPolyShapeIndex = polyShapeIndex-1;
        if(prevPolyShapeIndex<0) prevPolyShapeIndex = polyShape.size()-1;
        
        ezgl::point2d& currPoint = polyShape[polyShapeIndex];
        
        if(pointOutOfBounds && !visible.contains(currPoint.x, currPoint.y))  //Previous was out of bounds so we are out of bounds and we do nothing 
        {
            std::vector<ezgl::point2d> passedCorners = passedCorner(visible,polyShape[prevPolyShapeIndex], polyShape[polyShapeIndex]);
            if(passedCorners.empty()) continue;
            if(clippedPoly.back() == passedCorners.front()){ //If the last point was a corner then we don't want that corner again.
                clippedPoly.insert(clippedPoly.end(),passedCorners.begin()+1,passedCorners.end());
            }
            else{
                clippedPoly.insert(clippedPoly.end(),passedCorners.begin(),passedCorners.end());
            }
            continue; 
        }
        
        if(!pointOutOfBounds && visible.contains(currPoint.x, currPoint.y))//Previous was in this one is in so we just add it to the vector //Happens on first iteration always
        { 
            clippedPoly.push_back(currPoint);
            continue;
        }
        
        if(!visible.contains(currPoint.x, currPoint.y) && !pointOutOfBounds) //Exiting screen?
        { 
            pointOutOfBounds = true; 
            //Need to find the line connecting this point to the previous one and find it's intersection point with the screen boundary.
            ezgl::point2d& featPointInside = polyShape[prevPolyShapeIndex]; //Get prev point which was in map definitely exists
            ezgl::point2d& featPointOutside = polyShape[polyShapeIndex]; 
            ezgl::point2d screenInter = screenIntersection(visible, featPointInside, featPointOutside);
            
            clippedPoly.push_back(featPointInside);
            
            //clippedPointIndex.push_back(clippedPoly.size()); //The index where a clipping happened.
            clippedPoly.push_back(screenInter);
            
            continue; // We added the intersection of the boundary and the edge to the Shape vector we map continue.
        }
        
        if(pointOutOfBounds && visible.contains(currPoint.x, currPoint.y))   //Coming back to screen?
        {
            pointOutOfBounds = false;
            //Need to find the line connecting this point to the previous one and find it's intersection point with the screen boundary.
            ezgl::point2d& featPointInside = polyShape[polyShapeIndex];
            ezgl::point2d& featPointOutside = polyShape[prevPolyShapeIndex]; //Get prev point which was outside the map definitely exists
            ezgl::point2d screenInter = screenIntersection(visible, featPointInside, featPointOutside);
            
            //clippedPointIndex.push_back(clippedPoly.size()); //The index where a clipping happened.
            clippedPoly.push_back(screenInter);
            
            clippedPoly.push_back(featPointInside);
            continue; // We added the intersection of the boundary and the edge to the Shape vector we map continue.
        }
    }
    polyShape.push_back(polyShape.front()); //We kicked it out at first putting it back before returning
    return clippedPoly;
}

std::vector<ezgl::point2d> clipPolyDebug(ezgl::renderer &g, const ezgl::rectangle& visible, std::vector<ezgl::point2d> polyShape)
{
    if(polyShape.size()<2){
        std::vector<ezgl::point2d> empty;
        return empty;
    }
    polyShape.pop_back(); //Kick out the last element in the vector
    
    std::vector<ezgl::point2d> clippedPoly;
    std::vector<unsigned> clippedPointIndex;
    
    //ezgl::point2d startInter(0,0);
    //bool isStartInter = false;
    
    int startInterIndex = -1;
    
    bool pointOutOfBounds = false;
    
    for(unsigned i=0; i<polyShape.size(); i++){ //Find first point in rectangle
        if(visible.contains(polyShape[i].x, polyShape[i].y)){
            startInterIndex = i;
            break;
        }
    }
    if(startInterIndex == -1){ //All points were out of screen so we should draw the rectangle
        std::vector<ezgl::point2d> screenShape;
        /*screenShape.push_back(visible.top_left());
        screenShape.push_back(visible.bottom_left());
        screenShape.push_back(visible.bottom_right());
        screenShape.push_back(visible.top_right());
        screenShape.push_back(visible.top_left());*/
        return screenShape;
    }
    
    int polyShapeIndex = startInterIndex;
    for(unsigned i=0; i<=polyShape.size(); i++,polyShapeIndex++){ //We start with first in screen and end with it. (<=)
        if(polyShapeIndex==(int)polyShape.size())
            polyShapeIndex=0; //Make the index start at first point inside the screen and to wrap around
        
        int prevPolyShapeIndex = polyShapeIndex-1;
        if(prevPolyShapeIndex<0) prevPolyShapeIndex = polyShape.size()-1;
        
        ezgl::point2d& currPoint = polyShape[polyShapeIndex];
        
        if(pointOutOfBounds && !visible.contains(currPoint.x, currPoint.y))  //Previous was out of bounds so we are out of bounds and we do nothing 
        {
            std::vector<ezgl::point2d> passedCorners = passedCorner(visible,polyShape[prevPolyShapeIndex], polyShape[polyShapeIndex]);
            if(passedCorners.empty()) continue;
            if(clippedPoly.back() == passedCorners.front()){
                clippedPoly.insert(clippedPoly.end(),passedCorners.begin()+1,passedCorners.end());
            }
            else{
                clippedPoly.insert(clippedPoly.end(),passedCorners.begin(),passedCorners.end());
            }
            continue; 
        }
        
        if(!pointOutOfBounds && visible.contains(currPoint.x, currPoint.y))//Previous was in this one is in so we just add it to the vector //Happens on first iteration always
        { 
            clippedPoly.push_back(currPoint);
            continue;
        }
        
        if(!visible.contains(currPoint.x, currPoint.y) && !pointOutOfBounds) //Exiting screen?
        { 
            pointOutOfBounds = true; 
            //Need to find the line connecting this point to the previous one and find it's intersection point with the screen boundary.
            ezgl::point2d& featPointInside = polyShape[prevPolyShapeIndex]; //Get prev point which was in map definitely exists
            ezgl::point2d& featPointOutside = polyShape[polyShapeIndex]; 
            ezgl::point2d screenInter = screenIntersection(visible, featPointInside, featPointOutside);
            
            clippedPoly.push_back(featPointInside);
            
            clippedPointIndex.push_back(clippedPoly.size()); //The index where a clipping happened.
            clippedPoly.push_back(screenInter);
            
            g.set_color(ezgl::KHAKI);
            g.fill_arc(screenInter, 150, 0, 360);
            g.set_color(ezgl::RED);         //blue
            
            continue; // We added the intersection of the boundary and the edge to the Shape vector we map continue.
        }
        
        if(pointOutOfBounds && visible.contains(currPoint.x, currPoint.y))   //Coming back to screen?
        {
            pointOutOfBounds = false;
            //Need to find the line connecting this point to the previous one and find it's intersection point with the screen boundary.
            ezgl::point2d& featPointInside = polyShape[polyShapeIndex];
            ezgl::point2d& featPointOutside = polyShape[prevPolyShapeIndex]; //Get prev point which was outside the map definitely exists
            ezgl::point2d screenInter = screenIntersection(visible, featPointInside, featPointOutside);
            
            clippedPointIndex.push_back(clippedPoly.size()); //The index where a clipping happened.
            clippedPoly.push_back(screenInter);
            g.set_color(ezgl::GREEN);
            g.fill_arc(screenInter, 200, 0, 360);
            g.set_color(ezgl::RED);         //blue
                            
            
            clippedPoly.push_back(featPointInside);
            continue; // We added the intersection of the boundary and the edge to the Shape vector we map continue.
        }
    }
    
    for(unsigned j=0; j<clippedPoly.size(); j++){
        if(j==0 || j==clippedPoly.size()-1) g.set_color(ezgl::YELLOW);
        else g.set_color(255, 0, 0, 50);
        g.fill_arc(clippedPoly[j], 100, 0, 360);
        g.set_color(255, 0, 0, 50);
    }
    g.set_color(180, 200, 255, 100);         //blue
    
    return clippedPoly;
}

