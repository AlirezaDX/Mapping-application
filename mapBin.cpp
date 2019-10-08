/* 
 * File:   mapBin.cpp
 * Created on February 21, 2019, 12:04 AM
 */

#include "mapBin.h"
MapBin::MapBin() {
    
}


MapBin::~MapBin() {
}
void MapBin::clear(){
    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            bin[i][j].clear();
        }
        bin[i].clear();
        numberInBin[i].clear();
    }
    bin.clear();
    numberInBin.clear();
}

int MapBin::nFromX(double x){ 
            x= x-min_x;
            return (x/n_width);

        }
int MapBin::mFromY (double y){
    y=y-min_y;
    return y/m_width;
}

double MapBin::distance(double x1, double y1, double x2, double y2){
    return sqrt(pow((y2-y1),2) + pow((x2-x1),2));
}
double MapBin::distance(ezgl::point2d p1, ezgl::point2d p2){
    return sqrt(pow((p2.y-p1.y),2) + pow((p2.x-p1.x),2));
}


double MapBin::xFromLon (double lon){
    return EARTH_RADIUS_IN_METERS * DEG_TO_RAD * lon * cos(DEG_TO_RAD * latAvg);
}
double MapBin::yFromLat (double lat){
    return EARTH_RADIUS_IN_METERS * DEG_TO_RAD * lat;
}
ezgl::point2d MapBin::xyFromLatLon(LatLon position){
    ezgl::point2d ans(xFromLon(position.lon()), yFromLat(position.lat()));
    return ans;
}

double MapBin::lonFromX (double x){
    return x/(DEG_TO_RAD * cos(DEG_TO_RAD * latAvg)) / EARTH_RADIUS_IN_METERS;
}
double MapBin::latFromY (double y){
    return y/ DEG_TO_RAD / EARTH_RADIUS_IN_METERS;
}

LatLon MapBin::latLonFromXY(ezgl::point2d p){
    double lat = latFromY(p.y);
    double lon = lonFromX(p.x);
    LatLon ans(lat,lon);
    return ans;
}
LatLon MapBin::latLonFromXY(double x, double y){
    double lat = latFromY(y);
    double lon = lonFromX(x);
    LatLon ans(lat,lon);
    return ans;
}

bool MapBin::isInMap(LatLon position){
        return (position.lon() <= max_lon && position.lat() <= max_lat && position.lat() >=min_lat && position.lon() >= min_lon);
    }
bool MapBin::isInMap(double x, double y){
    return (x <= max_x && y <= max_y && y>= min_y && x>= min_x);
}
bool MapBin::isInMap(ezgl::point2d p){
    return isInMap(p.x, p.y);
}

bool MapBin::isInMap(int mIndex, int nIndex){
    return (mIndex < m && nIndex < n && nIndex>=0 && mIndex>=0);
}

bool MapBin::isInBin(double x, double y, int mIndex, int nIndex){
    if(!isInMap(x,y)) return false;
    return nFromX(x) != nIndex || mFromY(y) != mIndex;
}
bool MapBin::isInBin(ezgl::point2d myPoint, int mIndex, int nIndex){
    if(!isInMap(myPoint)) return false;
    return nFromX(myPoint.x) != nIndex || mFromY(myPoint.y) != mIndex;
}
bool MapBin::isInBin(LatLon myPosition, int mIndex, int nIndex){
    if(!isInMap(myPosition)) return false;
    ezgl::point2d myPoint = xyFromLatLon(myPosition);
    return isInBin(myPoint, mIndex, nIndex);
}

ezgl::point2d MapBin::binPos(int mIndex, int nIndex){
    ezgl::point2d ans( min_x + ((double)nIndex)*n_width, min_y + ((double)mIndex)*m_width);
    return ans;
}

//Returns an integer(1-9) representing which side of the bin the position is: 804 returns -1 if not in Bin
int MapBin::whereInBin(LatLon my_position, int mIndex,int nIndex){
    ezgl::point2d myPoint = xyFromLatLon(my_position);
    //If the position is not in the corresponding bin:
    if( !isInBin(my_position,mIndex,nIndex) ) return -1;
    
    //x/yPosBin would be the x and y of the point with respect to the bin (bottom left is (0,0))
    double xPosBin = myPoint.x - (min_x + nIndex * n_width); 
    double yPosBin = myPoint.y - (min_y + mIndex * m_width);
    
    //Let's see which side of the bin it is located //123
    double xStep = n_width/3.0;    //804
    double yStep = m_width/3.0;    //765
    if(xPosBin<xStep){
        if(yPosBin<yStep)
            return 1;
        if(yPosBin<2.0*yStep)       
            return 8;
        return 7;
    }
    if(xPosBin<2.0*xStep){
        if(yPosBin<yStep)
            return 2;
        if(yPosBin<2.0*yStep)       
            return 0;
        return 6;
    }
    //2*xStep<xPosBin<3*xStep:
    if(yPosBin<yStep)
        return 3;
    if(yPosBin<2.0*yStep)       
        return 4;
    return 5;
    }                  

std::pair<double,int> MapBin::findClosestInBin(LatLon my_position, int mIndex, int nIndex, LatLon (*positionFromID)(int)){
    //returns -1 if no point in the bin or out of bound.
    if( !isInMap(mIndex, nIndex) || bin[mIndex][nIndex].size()==0) return {-1,-1}; //If it doesn't exist.
    //Find the distance of the first point in that bin and assume as it's the closest.
    int closestInterID = bin[mIndex][nIndex][0]; 
    double closestDistance = find_distance_between_two_points(positionFromID(closestInterID), my_position);
    for(unsigned i=1; i<bin[mIndex][nIndex].size(); i++){
        int& competitorID = bin[mIndex][nIndex][i];
        LatLon competitorPosition = positionFromID(competitorID);
        double competitorDistance = find_distance_between_two_points(competitorPosition, my_position);
        if(competitorDistance < closestDistance){
            closestInterID = competitorID;
            closestDistance = competitorDistance;
        }
    }
    std::pair<double,int>  ans = {closestDistance, closestInterID};
    return ans;
}

std::pair<double,int> MapBin::findClosestInBin(double x, double y, int mIndex, int nIndex, LatLon (*positionFromID)(int)){
    ezgl::point2d p = {x,y};
    LatLon my_position(latLonFromXY(p));
    return findClosestInBin(my_position,mIndex,nIndex, positionFromID);
}

std::pair<double,int> MapBin::findClosestInBin(ezgl::point2d p, int mIndex, int nIndex, LatLon (*positionFromID)(int)){
    LatLon my_position(latLonFromXY(p));
    return findClosestInBin(my_position,mIndex,nIndex, positionFromID);
}

//This function uses a 3D data structure "bin" to find the closest intersection with 100% accuracy
//Limitation: If given point is out of map returns -1 (use another function.) It also returns -1 if there are no intersections up to the layer.
//Method: Bin divided the map to m*n pieces and stored intersection in each of them. This function finds the given positions location in this bin and starts
//starts its search from there. First it searches the corresponding bin and then all the bins around that bin layer by layer.
//The function will know the answer it found is accurate and returnable if it is less than the worst scenario case in which it is not accurate.
//The worst scenario is when the given point is at the top left corner of the bin and the closest intersection is M_width or N_width to it's top or left
//In this scenario their distance would be M_width or N_width therefore the for loop loops until distance < layer * min(M_width,N_width)
//In case all bins are searched(all layers) to find the answer it returns the answer.
int MapBin::layerFind(LatLon my_position, int numLayers, LatLon (*positionFromID)(int)){
    int mIndex = mFromY(yFromLat(my_position.lat()));
    int nIndex = nFromX(xFromLon(my_position.lon()));
    
    if ( !isInMap(mIndex, nIndex) ) //Out of map WIP*************************
        return -1;
   
    std::pair<double, int> winner = {-1,-1};//Invalid default value
    int layer;
    
    for(layer=1; (mIndex+layer < m || nIndex+layer < n || nIndex-layer>=0 || mIndex-layer>=0) && layer<=numLayers ; layer++){
        //To iterate through all layers of bin around the bin corresponding to given position.
        if(layer==1){//Only in its first layer we check for intersections at m & n (the bin corresponding to given position)
            std::pair<double,int> competitor = findClosestInBin(my_position, mIndex, nIndex, positionFromID);
            if(competitor.second != -1 && (competitor.first < winner.first || (winner.second == -1))) //If competitor is smaller than winner or winner is invalid
               winner = competitor;//Finding the smallest distance
        }
        
        for(int i=-layer; i<layer; i++){ //No equal sign because next for loop will check it.//Top side
            std::pair<double,int> competitor = findClosestInBin(my_position, mIndex+layer, nIndex+i, positionFromID);
            if(competitor.second != -1 && (competitor.first < winner.first || (winner.second == -1))) //If competitor is smaller than winner or winner is invalid
                winner = competitor;
        }
        for(int i=layer; i>-layer; i--){//Right side
            std::pair<double,int> competitor = findClosestInBin(my_position, mIndex+i, nIndex+layer, positionFromID);
            if(competitor.second != -1 && (competitor.first < winner.first || (winner.second == -1))) //If competitor is smaller than winner or winner is invalid
                winner = competitor;
        }
        for(int i=layer; i>-layer; i--){//Bottom side
            std::pair<double,int> competitor = findClosestInBin(my_position, mIndex-layer, nIndex+i, positionFromID);
            if(competitor.second != -1 && (competitor.first < winner.first || (winner.second == -1))) //If competitor is smaller than winner or winner is invalid
                winner = competitor;
        }
        for(int i=-layer; i<layer; i++){//Left side
            std::pair<double,int> competitor = findClosestInBin(my_position, mIndex+i, nIndex-layer, positionFromID);
            if(competitor.second != -1 && (competitor.first < winner.first || (winner.second == -1))) //If competitor is smaller than winner or winner is invalid
                winner = competitor;
        }

        if(winner.second != -1){ //Check if it found anything
            if(winner.first < std::min(n_width * layer, m_width * layer)) //Is our answer accurate?
                return winner.second;
        }
    } //For loop ends when all sides of the square we are searching are out of bounds
    return winner.second;
}
int MapBin::layerFind(double x, double y, int numLayers, LatLon (*positionFromID)(int)){
    LatLon my_position = latLonFromXY(x,y);
    return layerFind(my_position, numLayers, positionFromID);
}
int MapBin::layerFind(ezgl::point2d p, int numLayers, LatLon (*positionFromID)(int)){
    LatLon my_position = latLonFromXY(p);
    return layerFind(my_position, numLayers, positionFromID);
}

//Uses layerFind but goes through all it's layers
int MapBin::layerFind(LatLon my_position, LatLon (*positionFromID)(int)){
    
    int closestIntID = layerFind(my_position, std::max(m,n)/*To go through all layers*/, positionFromID); 
   
    if(closestIntID == -1) { //Error checking.
        std::cout << "Error in clickFind."<< std::endl;
    }
    return closestIntID;
}
int MapBin::layerFind(double x, double y, LatLon (*positionFromID)(int)){
    LatLon my_position = latLonFromXY(x,y);
    return layerFind(my_position, positionFromID);
}
int MapBin::layerFind(ezgl::point2d p, LatLon (*positionFromID)(int)){
    LatLon my_position = latLonFromXY(p);
    return layerFind(my_position, positionFromID);
}

//Uses layerFind with 1 layer to find the closest intersection
//Returns -1 if that intersection is not in range specified or if there are no intersections in the first layer of bin.
int MapBin::clickFind(LatLon my_position, float range, LatLon (*positionFromID)(int)){
    
    int closestIntID = layerFind(my_position, 1, positionFromID);
    if(closestIntID == -1) //Did not find an intersection.
        return -1;
    
    LatLon closestPosition = positionFromID(closestIntID);
    double closestDistance = find_distance_between_two_points(closestPosition, my_position);
    
    if(closestDistance > range)
        return -1;
    return closestIntID;
}
int MapBin::clickFind(double x, double y, float range, LatLon (*positionFromID)(int)){ 
    LatLon my_position = latLonFromXY(x,y);
    return clickFind(my_position, range, positionFromID);
}
int MapBin::clickFind(ezgl::point2d p, float range, LatLon (*positionFromID)(int)){ 
    LatLon my_position = latLonFromXY(p);
    return clickFind(my_position, range, positionFromID);
}

int MapBin::layersInRectangle(double x, double y, double width, double height){
    if(!isInMap(x,y)){ 
        return -1; //Limitation returns nothing
    }
    
    int numBinsN = width/2.0 /n_width;
    int numBinsM = height/2.0 /m_width;
  
    int binsToCheck = std::max(numBinsN, numBinsM);
    int layers;
    //if(binsToCheck == 0)
    //    layers = 1; //minimum is one for intersectionsInLayer
    return layers = binsToCheck + 1;   //Some geometry happening here to get this value.
                                //Worst case scenario the middle point is at the corner of a bin
                                //in this case max(numBinsM,N) is indicating how many more bins it occupies on the other side 
                                //if it is zero it means it can be in layer 1 if 1 it can be on layer 2 and so on.
}
int MapBin::layersInRectangle(ezgl::point2d center, double width, double height){
    return layersInRectangle(center.x, center.y, width, height);
}

std::vector<int> MapBin::IDsInLayers(double x, double y, int numLayers){
    
    int mIndex = mFromY(y);
    int nIndex = nFromX(x);
    
    if ( !isInMap(mIndex, nIndex) ){ //Out of map does not work
        std::vector<int> empty;
        return empty;
    }
    std::vector<int> IDsInLayers;
    IDsInLayers.reserve(countIDsInLayers(x,y,numLayers));
    for(unsigned j=0; j<bin[mIndex][nIndex].size(); j++){ //Adding intersections in the bin corresponding to the intersection
                IDsInLayers.push_back(bin[mIndex][nIndex][j]);//Middle
            }
    
    int layer;//
    for(layer=1; (mIndex+layer < m || nIndex+layer < n || nIndex-layer>=0 || mIndex-layer>=0) && layer<=numLayers  ; layer++){
        //To iterate through all layers of bin around the bin which corresponds to given position.

        for(int i=-layer; i<layer; i++){ //No equal sign because next for loop will check it. //Top side
            for(unsigned j=0; isInMap(mIndex+layer, nIndex+i) && j<bin[mIndex+layer][nIndex+i].size(); j++){
                IDsInLayers.push_back(bin[mIndex+layer][nIndex+i][j]);//Top side
            }
        }
        for(int i=layer; i>-layer; i--){//Right side
            for(unsigned j=0; isInMap(mIndex+i, nIndex+layer) && j<bin[mIndex+i][nIndex+layer].size(); j++){
                IDsInLayers.push_back(bin[mIndex+i][nIndex+layer][j]);//Right side
            }
        }
        for(int i=layer; i>-layer; i--){//Bottom side
            for(unsigned j=0; isInMap(mIndex-layer, nIndex+i) && j<bin[mIndex-layer][nIndex+i].size(); j++){
                IDsInLayers.push_back(bin[mIndex-layer][nIndex+i][j]);//Bottom side
            }
        }
        for(int i=-layer; i<layer; i++){//Left side
            for(unsigned j=0; isInMap(mIndex+i, nIndex-layer) && j<bin[mIndex+i][nIndex-layer].size(); j++){
                IDsInLayers.push_back(bin[mIndex+i][nIndex-layer][j]);//Left side
            }
        }

    } //For loop ends when all sides of the square we are searching are out of bounds
    return IDsInLayers;
}
std::vector<int> MapBin::IDsInLayers(ezgl::point2d p, int layers){
    return IDsInLayers(p.x, p.y, layers);
}

std::vector<int> MapBin::IDsInRectangle(double x, double y, double width, double height){
    if(!isInMap(x,y)){ //If the center is out of map
        /*x= (_DATA.max_x - _DATA.min_x)/ 2.0;
        y= (_DATA.max_y - _DATA.min_y)/ 2.0;
        width = _DATA.x_length;
        height = _DATA.y_length;*/
        return IDsInLayers(x,y,1); //Limitation returns nothing
    }
    
    int numBinsN = width/2.0 /n_width;
    int numBinsM = height/2.0 /m_width;
    
    
    int binsToCheck = std::max(numBinsN, numBinsM);
    int layers;
    //if(binsToCheck == 0)
    //    layers = 1; //minimum is one for intersectionsInLayer
    layers = binsToCheck + 1;   //Some geometry happening here to get this value.
                                //Worst case scenario the middle point is at the corner of a bin
                                //in this case max(numBinsM,N) is indicating how many more bins it occupies on the other side 
                                //if it is zero it means it can be in layer 1 if 1 it can be on layer 2 and so on.
    return IDsInLayers(x,y,layers);
}
std::vector<int> MapBin::IDsInRectangle(ezgl::point2d center, double width, double height){
    return IDsInRectangle(center.x, center.y, width, height);
}

int MapBin::countIDsInLayers(double x, double y, int numLayers){
    int mIndex = mFromY(y);
    int nIndex = nFromX(x);
    if ( !isInMap(mIndex, nIndex) ){ //Out of map does not work
        return -1;
    }
    int ans = 0;
    ans += numberInBin[mIndex][nIndex];          
    int layer;
    for(layer=1; (mIndex+layer < m || nIndex+layer < n || nIndex-layer>=0 || mIndex-layer>=0) && layer<=numLayers  ; layer++){
        //To iterate through all layers of bin around the bin which corresponds to given position.

        for(int i=-layer; i<layer; i++){ //No equal sign because next for loop will check it. //Top side
            if(isInMap(mIndex+layer, nIndex+i))
                ans += numberInBin[mIndex+layer][nIndex+i];
        }
        for(int i=layer; i>-layer; i--){//Right side
            if(isInMap(mIndex+i, nIndex+layer))
                ans += numberInBin[mIndex+i][nIndex+layer];
        }
        for(int i=layer; i>-layer; i--){//Bottom side
            if(isInMap(mIndex-layer, nIndex+i))
                ans += numberInBin[mIndex-layer][nIndex+i];
        }
        for(int i=-layer; i<layer; i++){//Left side
            if(isInMap(mIndex+i, nIndex-layer))
                ans += numberInBin[mIndex+i][nIndex-layer];
        }
    } 
    return ans;
}
int MapBin::countIDsInLayers(ezgl::point2d p, int layers){
    return countIDsInLayers(p.x, p.y, layers);
}

int MapBin::countIDsInRectangle(double x, double y, double width, double height){
    int layers = layersInRectangle(x,y,width,height); 
    return countIDsInLayers(x,y,layers);
}
int MapBin::countIDsInRectangle(ezgl::point2d center, double width, double height){
    return countIDsInRectangle(center.x, center.y, width, height);
}

std::vector<int> MapBin::uniqueIDsInLayers(double x, double y, int numLayers){
    
    int mIndex = mFromY(y);
    int nIndex = nFromX(x);
    
    if ( !isInMap(mIndex, nIndex) ){ //Out of map does not work
        std::vector<int> empty;
        return empty;
    }
    //We will flag the IDs that are in the layers to avoid duplicates
    std::vector<bool> pushedFlag(max_ID, false);
    int count = 0;
    
    for(unsigned j=0; j<bin[mIndex][nIndex].size(); j++){ //Adding intersections in the bin corresponding to the intersection          
        if(pushedFlag[bin[mIndex][nIndex][j]] == false){
            pushedFlag[bin[mIndex][nIndex][j]] =true;//Middle
            count++;
        }
    }
    
    int layer;//
    for(layer=1; (mIndex+layer < m || nIndex+layer < n || nIndex-layer>=0 || mIndex-layer>=0) && layer<=numLayers  ; layer++){
        //To iterate through all layers of bin around the bin which corresponds to given position.

        for(int i=-layer; i<layer; i++){ //No equal sign because next for loop will check it. //Top side
            for(unsigned j=0; isInMap(mIndex+layer, nIndex+i) && j<bin[mIndex+layer][nIndex+i].size(); j++){
                if(pushedFlag[bin[mIndex+layer][nIndex+i][j]] == false){
                    pushedFlag[bin[mIndex+layer][nIndex+i][j]] =true;//Top side
                    count++;
                }
            }
        }
        for(int i=layer; i>-layer; i--){//Right side
            for(unsigned j=0; isInMap(mIndex+i, nIndex+layer) && j<bin[mIndex+i][nIndex+layer].size(); j++){
                if(pushedFlag[bin[mIndex+i][nIndex+layer][j]] == false){
                    pushedFlag[bin[mIndex+i][nIndex+layer][j]] = true;//Right side
                    count++;
                }
            }
        }
        for(int i=layer; i>-layer; i--){//Bottom side
            for(unsigned j=0; isInMap(mIndex-layer, nIndex+i) && j<bin[mIndex-layer][nIndex+i].size(); j++){
                if(pushedFlag[bin[mIndex-layer][nIndex+i][j]] == false){
                    pushedFlag[bin[mIndex-layer][nIndex+i][j]] = true;//Bottom side
                    count++;
                }
            }
        }
        for(int i=-layer; i<layer; i++){//Left side
            for(unsigned j=0; isInMap(mIndex+i, nIndex-layer) && j<bin[mIndex+i][nIndex-layer].size(); j++){
                if(pushedFlag[bin[mIndex+i][nIndex-layer][j]] == false){
                    pushedFlag[bin[mIndex+i][nIndex-layer][j]] = true;//Left side
                    count++;
                }
            }
        }
    } 
    std::vector<int> uniqueIDsInLayers; //What we will return
    uniqueIDsInLayers.reserve(count);
    for(unsigned i=0; i<pushedFlag.size(); i++)
        if(pushedFlag[i]==true) //Goes through all IDs and push only the ones that are true resulting in a sorted and unique data
            uniqueIDsInLayers.push_back(i);
    return uniqueIDsInLayers;
}
std::vector<int> MapBin::uniqueIDsInLayers(ezgl::point2d p, int layers){
    return uniqueIDsInLayers(p.x, p.y, layers);
}

std::vector<int> MapBin::uniqueIDsInRectangle(double x, double y, double width, double height){
    
    int layers = layersInRectangle(x,y,width,height); 
    return uniqueIDsInLayers(x,y,layers);
}
std::vector<int> MapBin::uniqueIDsInRectangle(ezgl::point2d center, double width, double height){
    return uniqueIDsInRectangle(center.x, center.y, width, height);
}

std::vector<int> MapBin::fineIDsInRectangle(double x, double y, double width, double height){
    if( floor(width/n_width) != ceil(width/n_width) || floor(height/m_width) != ceil(height/m_width) ){
        
        std::vector<int> empty;
        return empty;                
    }
    double topLeftBinX = x - width/2.0;
    double topLeftBinY = y + height/2.0 - m_width;
    int topLeftN = nFromX(topLeftBinX);
    int topLeftM = mFromY(topLeftBinY);
    
    double numBinsX = width/n_width;
    double numBinsY = height/m_width;
    std::vector<int> ans;
    ans.reserve(numBinsX * numBinsY);
    for(int i=0; i<numBinsY; i++){
        for(int j=0; j<numBinsX; j++){
            int binN = topLeftN + j;
            int binM = topLeftM - i;
            if(isInMap(binM,binN))
                for(unsigned k=0; k<bin[binM][binN].size(); k++){
                    ans.push_back(bin[binM][binN][k]);
                }
        }
    }
    return ans;
}

groupedBins MapBin::fineIDsInGroups(double x, double y, double width, double height, LatLon (*positionFromID)(int)){
    if( floor(width/n_width) != ceil(width/n_width) || floor(height/m_width) != ceil(height/m_width) ){
        std::cout << "Rectangle bad size." << "\n";
        groupedBins empty;
        return empty;                
    }
    if((x-width/2.0-min_x)/((double)(n)) != (x-width/2.0-min_x)/n || (y+height/2.0-m_width-min_y)/((double)(m)) != (y+height/2.0-m_width-min_y)/m )
        std::cout << "Rectangle off bins. \n";
    double topLeftBinX = x - width/2.0;
    double topLeftBinY = y + height/2.0 - m_width;
    int topLeftN = nFromX(topLeftBinX);
    int topLeftM = mFromY(topLeftBinY);
    
    double numBinsX = width/n_width;
    double numBinsY = height/m_width;
    double avgX = 0;
    double avgY = 0;
    int countAvg = 0; //to be divided in the end to achieve average
    std::vector<int> ans;
    ans.reserve(numBinsX * numBinsY);
    for(int i=0; i<numBinsY; i++){
        for(int j=0; j<numBinsX; j++){
            int binN = topLeftN + j;
            int binM = topLeftM - i;
            if(isInMap(binM,binN))
                for(unsigned k=0; k<bin[binM][binN].size(); k++){
                    ans.push_back(bin[binM][binN][k]);
                    LatLon idPos = positionFromID(bin[binM][binN][k]);
                    avgX = avgX + xFromLon(idPos.lon());
                    avgY = avgY + yFromLat(idPos.lat());
                    countAvg++;
                }
        }
    }
    avgX/=countAvg;
    avgY/=countAvg;
    //avgX = (avgX + x)/2.0;
    //avgY = (avgY + y)/2.0;        
    return {avgX, avgY,ans};
}


//Range can not be less than min(m/n_width) or u get duplicates
std::vector<groupedBins> MapBin::groupedBinsInRectangle(double x, double y, double width, double height, double range, LatLon (*positionFromID)(int)){
    if(!isInMap(x,y)){
        std::vector<groupedBins> empty;
        return empty;
    }
    
    double topLeftX = x - width/2.0;
    double topLeftY = y + height/2.0;
    
    //nFromX and mFromY round down
    ezgl::point2d topLeftBin = binPos(mFromY(topLeftY), nFromX(topLeftX));
    double visionOffX = topLeftX - topLeftBin.x; //How off is the vision we have from the first bin(great if zero /kinda)
    double visionOffY = (topLeftBin.y + m_width) - topLeftY; //Always a positive value
    
    int numBinsX = round(range*2.0/n_width); //Define the closest rectangle that resembeles the range (group)
    int numBinsY = round(range*2.0/m_width); //Using complete bins ONLY (not half a bin)
    if( visionOffY<0 || visionOffX<0){
        std::cout << "Error 1." << "\n";
        std::vector<groupedBins> empty;
        return empty;
    }
    if(numBinsX==0 || numBinsY==0 ){
        //Do bare minimum
        numBinsX = 1;
        numBinsY = 1;
        std::cout << "Range top small. Doing min for POI grouping.";
    }
         
                            
    double groupWidth = numBinsX * n_width;
    double groupHeight = numBinsY * m_width; 
    
    //Grouping starts from the top left bin which can be partially out of screen
    //double topLeftOfGroupingX = topLeftBin.x;
    //double topLeftOfGroupingY = topLeftBin.y + m_width;
    int numberOfGroupsInX = ceil((width + visionOffX) / groupWidth);
    int numberOfGroupsInY = ceil((height + visionOffY) / groupHeight);
    
   
    double topLeftGroupCenterX = topLeftBin.x + groupWidth/2.0;
    double topLeftGroupCenterY = topLeftBin.y + m_width - groupHeight/2.0;
    //Now we know how many groups we have in 2D:
    std::vector<groupedBins> groupedBinsInRectangle;
    groupedBinsInRectangle.reserve(numberOfGroupsInX * numberOfGroupsInY);
    for(int i=0; i<numberOfGroupsInY; i++){
        for(int j=0; j<numberOfGroupsInX; j++){
            groupedBinsInRectangle.push_back( fineIDsInGroups(topLeftGroupCenterX + (j*groupWidth),
                    topLeftGroupCenterY - (i*groupHeight),
                    groupWidth, groupHeight, positionFromID) );  
        }
    }
    return groupedBinsInRectangle;
}
std::vector<groupedBins> MapBin::groupedBinsInRectangle(ezgl::point2d center, double width, double height, double range, LatLon (*positionFromID)(int)){
    return groupedBinsInRectangle(center.x, center.y, width, height, range,positionFromID);
}