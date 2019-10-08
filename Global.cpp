#include "Global.h"

void Global::loadStructures(){
    loadStreetStructures(); //Must be done before IntersectionStructures because seginfo is used
    
    loadStreetOSMData();
    
    loadIntersectionStructures();
    
    loadIntersectionMap();//Must load intersections first.
    
    loadPrimaryInterMap();
    
    loadPOIMap();
    
    loadFeaturesArea(); 
    
    loadFeatureMap(); //Must loadFeatureArea first
    
    loadFeatureZoomedMap(); //Must loadFeatureArea first
    
    loadStreetNameDictionary();
    
    loadRailwayOSMData();    
    
    loadHotelsOSMData();
    
    loadCityLatLonData();
    
    loadWeatherData();
}

void Global::clearStructures(){ 
    //intersectionStreetSegment:
    for(unsigned intersectID = 0; intersectID < intersectionStreetSegment.size(); ++intersectID){
        intersectionStreetSegment[intersectID].clear();
    }

    //streetIntersection & streetStreetSegment & streetStreetSegment:
    for(unsigned streetID = 0; streetID < streetIntersection.size(); ++streetID){
        streetIntersection[streetID].clear();
        streetStreetSegment[streetID].clear();
        streetStreetSegment[streetID].clear();
    }
    
    //Clearing MapBin objects
    intersectionMap.clear();
    primaryInterMap.clear();
    poiMap.clear();
    featureMap.clear();
    featureZoomedMap.clear();
    
    //intersectionStreetStruct:
    for(unsigned i=0; i<intersectionStreetStruct.size(); i++){
        intersectionStreetStruct[i].primaryStreets.clear();
        intersectionStreetStruct[i].secondaryStreets.clear();
        intersectionStreetStruct[i].tertiaryStreets.clear();
    }
    intersectionStreetStruct.clear();
    
    for(unsigned i=0; i<intersectionIntersection.size(); i++){
        intersectionIntersection[i].clear();
    }
    intersectionIntersection.clear();
    
    intersectionStreetSegment.clear();
    mainIntersections.clear();
    streetStreetSegment.clear();
    streetStreetSegment.clear();
    streetSegmentInfo.clear();
    streetSegmentTravelTime.clear();
    streetIntersection.clear();
    featIDsAreaSorted.clear();
    streetOSMData.clear();
    subwayStationOSMData.clear();
    hotelsOSMData.clear();
    cityLatLonData.clear();
    weatherData.clear();
    clearDictionary();
    maxSpeedLimit = 0;
    // Reset max/min lat/lon if needed.
}

void Global::loadIntersectionStructures(){ //intersectionStreetSegment & intersectionIntersection & max/min_lat/lon
    int intesectionNumbers = getNumIntersections();
    intersectionIntersection.resize(intesectionNumbers); 
    intersectionStreetSegment.resize(intesectionNumbers);
    intersectionStreetStruct.resize(intesectionNumbers);
    
    if(intesectionNumbers>0){
        max_lat=getIntersectionPosition(0).lat();
        min_lat=getIntersectionPosition(0).lat();
        max_lon=getIntersectionPosition(0).lon();
        min_lon=getIntersectionPosition(0).lon();
    }
                
    //Find all street segments connected to an intersection
    for(IntersectionIndex intersectID = 0; intersectID < intesectionNumbers; ++intersectID){ //Loop through intersections
        int numOfIntersectionsSegments = getIntersectionStreetSegmentCount(intersectID);
        
        for(int segmentNumber = 0; segmentNumber < numOfIntersectionsSegments; ++segmentNumber){ //Loops through street segments of the intersection
            StreetSegmentIndex streetSegmentsID = getIntersectionStreetSegment(segmentNumber, intersectID);
            //intersectionStreetSegment:
            intersectionStreetSegment[intersectID].push_back(streetSegmentsID);
            
            //intersectionIntersection:
            InfoStreetSegment& segInfo = streetSegmentInfo[streetSegmentsID]; //Reference for ease of read 
            
            if(intersectID == segInfo.from){ //We may add so that it holds a flag as if it's one way
                intersectionIntersection[intersectID].insert(segInfo.to); 
            }
            else if(intersectID == segInfo.to) //If the segment holds the intersection as to then we can only add if it is not one way.
                if(!segInfo.oneWay)
                    intersectionIntersection[intersectID].insert(segInfo.from);
            
            //assign street segments connected to an intersection to different categories
            if((streetOSMData.find(segInfo.wayOSMID)->second.second == "motorway")
                    || (streetOSMData.find(segInfo.wayOSMID)->second.second == "trunk")
                    || (streetOSMData.find(segInfo.wayOSMID)->second.second == "primary")
                    || (streetOSMData.find(segInfo.wayOSMID)->second.second == "secondary")
                    || (streetOSMData.find(segInfo.wayOSMID)->second.second == "motorway_link")
                    || (streetOSMData.find(segInfo.wayOSMID)->second.second == "trunk_link")){
                intersectionStreetStruct[intersectID].primaryStreets.push_back(streetSegmentsID);
                    mainIntersections.push_back(intersectID);//Load mainIntersections
            }
            else if(streetOSMData.find(segInfo.wayOSMID)->second.second == "tertiary")
                intersectionStreetStruct[intersectID].secondaryStreets.push_back(streetSegmentsID);
            else
                intersectionStreetStruct[intersectID].tertiaryStreets.push_back(streetSegmentsID);
        }
        
        //max/min of lat/lon
        LatLon intersectionPosition = getIntersectionPosition(intersectID);
        if(intersectionPosition.lat() > max_lat)
            max_lat = intersectionPosition.lat();
        if(intersectionPosition.lat() < min_lat)
            min_lat = intersectionPosition.lat();
        if(intersectionPosition.lon() > max_lon)
            max_lon = intersectionPosition.lon();
        if(intersectionPosition.lon() < min_lon)
            min_lon = intersectionPosition.lon();
    }
    latAvg= (max_lat+min_lat)/2.0;
    max_x = xFromLon(max_lon);
    max_y = yFromLat(max_lat);
    min_x = xFromLon(min_lon);
    min_y = yFromLat(min_lat);
    x_length = max_x - min_x;
    y_length = max_y - min_y;
            
}

void Global::loadIntersectionMap(){
    //Giving the object it's local values:
    intersectionMap.m_width = BIN_INTERSECTION_M_WIDTH;
    intersectionMap.n_width = BIN_INTERSECTION_N_WIDTH;
    intersectionMap.m = y_length/intersectionMap.m_width +1;
    intersectionMap.n = x_length/intersectionMap.n_width +1;
    intersectionMap.x_length = x_length;
    intersectionMap.y_length = y_length;
    intersectionMap.max_x = max_x;
    intersectionMap.max_y = max_y;
    intersectionMap.min_x = min_x;
    intersectionMap.min_y = min_y;
    intersectionMap.latAvg = latAvg;
    intersectionMap.max_lat = max_lat;
    intersectionMap.min_lat = min_lat;
    intersectionMap.max_lon = max_lon;
    intersectionMap.min_lon = min_lon;
    intersectionMap.bin.resize(intersectionMap.m, vector<vector<int>>(intersectionMap.n)); //Resize the 2D vector to correspond to m and n
    intersectionMap.numberInBin.resize(intersectionMap.m, vector<int>(intersectionMap.n, 0));
    int interNumber = getNumIntersections();
    intersectionMap.max_ID = interNumber;
    for(IntersectionIndex interID = 0; interID < interNumber; ++interID){ //Loop through intersections place them into bins
        int nIndex = intersectionMap.nFromX(xFromLon(getIntersectionPosition(interID).lon()));
        int mIndex = intersectionMap.mFromY(yFromLat(getIntersectionPosition(interID).lat()));
        intersectionMap.bin[mIndex][nIndex].push_back(interID);
        intersectionMap.numberInBin[mIndex][nIndex]++;
    }
}

void Global::loadPrimaryInterMap(){
    //Giving the object it's local values:
    primaryInterMap.m_width = BIN_PRIMEINTER_M_WIDTH;
    primaryInterMap.n_width = BIN_PRIMEINTER_N_WIDTH;
    primaryInterMap.m = y_length/primaryInterMap.m_width +1;
    primaryInterMap.n = x_length/primaryInterMap.n_width +1;
    primaryInterMap.x_length = x_length;
    primaryInterMap.y_length = y_length;
    primaryInterMap.max_x = max_x;
    primaryInterMap.max_y = max_y;
    primaryInterMap.min_x = min_x;
    primaryInterMap.min_y = min_y;
    primaryInterMap.latAvg = latAvg;
    primaryInterMap.max_lat = max_lat;
    primaryInterMap.min_lat = min_lat;
    primaryInterMap.max_lon = max_lon;
    primaryInterMap.min_lon = min_lon;
    primaryInterMap.bin.resize(primaryInterMap.m, vector<vector<int>>(primaryInterMap.n)); //Resize the 2D vector to correspond to m and n
    primaryInterMap.numberInBin.resize(primaryInterMap.m, vector<int>(primaryInterMap.n, 0));
    int interNumber = mainIntersections.size();
    primaryInterMap.max_ID = interNumber;
    for(IntersectionIndex interID = 0; interID < interNumber; ++interID){ //Loop through intersections place them into bins
        int nIndex = primaryInterMap.nFromX(xFromLon(getIntersectionPosition(mainIntersections[interID]).lon()));
        int mIndex = primaryInterMap.mFromY(yFromLat(getIntersectionPosition(mainIntersections[interID]).lat()));
        primaryInterMap.bin[mIndex][nIndex].push_back(mainIntersections[interID]);
        primaryInterMap.numberInBin[mIndex][nIndex]++;
    }
}

void Global::loadPOIMap(){
    //Giving the object it's local values:
    poiMap.m_width = BIN_POI_M_WIDTH;
    poiMap.n_width = BIN_POI_N_WIDTH;
    poiMap.m = y_length/poiMap.m_width +1;
    poiMap.n = x_length/poiMap.n_width +1;
    poiMap.x_length = x_length;
    poiMap.y_length = y_length;
    poiMap.max_x = max_x;
    poiMap.max_y = max_y;
    poiMap.min_x = min_x;
    poiMap.min_y = min_y;
    poiMap.latAvg = latAvg;
    poiMap.max_lat = max_lat;
    poiMap.min_lat = min_lat;
    poiMap.max_lon = max_lon;
    poiMap.min_lon = min_lon;
    poiMap.bin.resize(poiMap.m, vector<vector<int>>(poiMap.n)); //Resize the 2D vector to correspond to m and n
    poiMap.numberInBin.resize(poiMap.m, vector<int>(poiMap.n, 0));
    int POINumbers = getNumPointsOfInterest();
    poiMap.max_ID = POINumbers;
    for(POIIndex POIID = 0; POIID < POINumbers; ++POIID){ //Loop through intersections place them into bins
        LatLon POIPoint = getPointOfInterestPosition(POIID);
        if(isInMap(POIPoint)){
            int nIndex = poiMap.nFromX(xFromLon(POIPoint.lon()));
            int mIndex = poiMap.mFromY(yFromLat(POIPoint.lat()));
            poiMap.bin[mIndex][nIndex].push_back(POIID);
            poiMap.numberInBin[mIndex][nIndex]++;
        }
    }
}

void Global::loadFeaturesArea(){
    std::multimap<double, int>  featuresAreaMultiMap; //To sort features based on area
    vector<int>                 openFeatures; //To hold all the openFeature IDs to be pushed back all at once(0 area)
    for(int featID=0; featID<getNumFeatures(); featID++)
    {//Go through the features to sort them in a map based on area
        if(/*ifClosed*/getFeaturePoint(0, featID).lat() == getFeaturePoint(getFeaturePointCount(featID)-1 , featID).lat() && getFeaturePoint(0, featID).lon() == getFeaturePoint(getFeaturePointCount(featID)-1 , featID).lon()){
            double area;
            typedef bg::model::point<float,2,bg::cs::cartesian> point_t;
            typedef bg::model::ring<point_t> ring_t; //Make a ring out of the feature boundaries and compute the area 
            ring_t myRing; 
            for(int points=0; points<getFeaturePointCount(featID); points++){
                bg::append(myRing, point_t(xFromLon(getFeaturePoint(points, featID).lon()), yFromLat(getFeaturePoint(points, featID).lat())));
            }
            area = fabs(bg::area(myRing));//compute the area 
            featuresAreaMultiMap.insert({area, featID}); //Sort by area
        }
        else{
            openFeatures.push_back(featID); 
        }
    }
    featIDsAreaSorted.reserve(getNumFeatures());
    for(unsigned i=0; i<openFeatures.size(); i++){ //The vector will have features in ascending order of area
        featIDsAreaSorted.push_back(openFeatures[i]);
    }
    for(auto it=featuresAreaMultiMap.begin(); it!=featuresAreaMultiMap.end(); it++){//The vector will have features in ascending order of area
        featIDCloseBegin = openFeatures.size();
        featIDsAreaSorted.push_back(it->second);
    }
}

void Global::loadFeatureMap(){ 
    //Giving the object it's local values:
    featureMap.m_width = BIN_FEATURE_M_WIDTH;
    featureMap.n_width = BIN_FEATURE_M_WIDTH;
    featureMap.m = y_length/featureMap.m_width +1;
    featureMap.n = x_length/featureMap.n_width +1;
    featureMap.x_length = x_length;
    featureMap.y_length = y_length;
    featureMap.max_x = max_x;
    featureMap.max_y = max_y;
    featureMap.min_x = min_x;
    featureMap.min_y = min_y;
    featureMap.latAvg = latAvg;
    featureMap.max_lat = max_lat;
    featureMap.min_lat = min_lat;
    featureMap.max_lon = max_lon;
    featureMap.min_lon = min_lon;
    //Resize the 2D vector to correspond to m and n
    featureMap.bin.resize(featureMap.m, vector<vector<int>>(featureMap.n)); 
    std::vector<vector<set<int>>> mapSet(featureMap.m, vector<set<int>>(featureMap.n));
    featureMap.numberInBin.resize(featureMap.m, vector<int>(featureMap.n, 0));
   
    int featureNumber = getNumFeatures();
    featureMap.max_ID = featureNumber;
    for(FeatureIndex featureIndex = 0; featureIndex < featureNumber; ++featureIndex){ //Loop through features place them into bins
        for(int featurePointIndex =0; featurePointIndex < getFeaturePointCount(featIDsAreaSorted[featureIndex]); featurePointIndex++){ //Go through all points of the feature
            LatLon featurePoint = getFeaturePoint(featurePointIndex, featIDsAreaSorted[featureIndex]);
            if(isInMap(featurePoint)){
                int nIndex = featureMap.nFromX(xFromLon(featurePoint.lon()));
                int mIndex = featureMap.mFromY(yFromLat(featurePoint.lat()));
                mapSet[mIndex][nIndex].insert(featureIndex); //Insert IDs of the feature to any bin which includes the boundaries of the feature
                featureMap.numberInBin[mIndex][nIndex]++; //A set is used to avoid duplicates.
            }
        }
    }
    for(int mIndex=0; mIndex < featureMap.m; mIndex++){
        for(int nIndex=0; nIndex < featureMap.n; nIndex++){
            featureMap.bin[mIndex][nIndex].resize(mapSet[mIndex][nIndex].size());
            std::copy(mapSet[mIndex][nIndex].begin(), mapSet[mIndex][nIndex].end(), 
                    featureMap.bin[mIndex][nIndex].begin()); //Copies the set into the vector
        }
    }
}

void Global::loadFeatureZoomedMap(){
    //Giving the object it's local values:
    featureZoomedMap.m_width = BIN_FEATURE_ZOOMED_M_WIDTH;
    featureZoomedMap.n_width = BIN_FEATURE_ZOOMED_M_WIDTH;
    featureZoomedMap.m = y_length/featureZoomedMap.m_width +1;
    featureZoomedMap.n = x_length/featureZoomedMap.n_width +1;
    featureZoomedMap.x_length = x_length;
    featureZoomedMap.y_length = y_length;
    featureZoomedMap.max_x = max_x;
    featureZoomedMap.max_y = max_y;
    featureZoomedMap.min_x = min_x;
    featureZoomedMap.min_y = min_y;
    featureZoomedMap.latAvg = latAvg;
    featureZoomedMap.max_lat = max_lat;
    featureZoomedMap.min_lat = min_lat;
    featureZoomedMap.max_lon = max_lon;
    featureZoomedMap.min_lon = min_lon;
    featureZoomedMap.bin.resize(featureZoomedMap.m, vector<vector<int>>(featureZoomedMap.n)); //Resize the 2D vector to correspond to m and n
    featureZoomedMap.numberInBin.resize(featureZoomedMap.m, vector<int>(featureZoomedMap.n, 0));
    int featureNumber = getNumFeatures();
    featureZoomedMap.max_ID = featureNumber;
    for(FeatureIndex featureIndex = 0; featureIndex < featureNumber; ++featureIndex){ //Loop through features to place them into bins
        //First we find the max/min m/n and then place the ID in all bins between max/n m/n
        //Please note that we use featIDsAreaSorted to get the feature IDs in a sorted manner. We push this sorted index so that when extracted the area sorting is conserved.
        int numPoints = getFeaturePointCount(featIDsAreaSorted[featureIndex]); 
        double maxLat = getFeaturePoint(0, featIDsAreaSorted[featureIndex]).lat();
        double minLat = getFeaturePoint(0, featIDsAreaSorted[featureIndex]).lat();
        double maxLon = getFeaturePoint(0, featIDsAreaSorted[featureIndex]).lon();
        double minLon = getFeaturePoint(0, featIDsAreaSorted[featureIndex]).lon();
        for(int featurePointIndex =1; featurePointIndex < numPoints; featurePointIndex++){
            LatLon compitetorPoint = getFeaturePoint(featurePointIndex, featIDsAreaSorted[featureIndex]); 
            maxLat = std::max(maxLat, compitetorPoint.lat());
            minLat = std::min(minLat, compitetorPoint.lat());
            maxLon = std::max(maxLat, compitetorPoint.lon());
            minLon = std::min(minLon, compitetorPoint.lon());
        } //Found max/min lat/lon
        int maxM = featureZoomedMap.mFromY(yFromLat(maxLat));
        int minM = featureZoomedMap.mFromY(yFromLat(minLat));
        int maxN = featureZoomedMap.nFromX(xFromLon(maxLon));
        int minN = featureZoomedMap.nFromX(xFromLon(minLon));
        //Some points might be out of map so:
        if(maxM > featureZoomedMap.m-1) maxM = featureZoomedMap.m-1;
        if(maxM < 0) maxM = 0;
        if(minM > featureZoomedMap.m-1) minM = featureZoomedMap.m-1;
        if(minM < 0) minM = 0;
        if(maxN > featureZoomedMap.n-1) maxN = featureZoomedMap.n-1;
        if(maxN < 0) maxN = 0;
        if(minN > featureZoomedMap.n-1) minN = featureZoomedMap.n-1;
        if(minN < 0) minN = 0;
        
        //place the "sorted IDs" in all bins between max/n m/n
        for(int mIndex = minM; mIndex <= maxM; mIndex++){
            for(int nIndex = minN; nIndex <= maxN; nIndex++){
                featureZoomedMap.bin[mIndex][nIndex].push_back(featureIndex);
                featureZoomedMap.bin[mIndex][nIndex];
                featureZoomedMap.numberInBin[mIndex][nIndex]++;
            }
        }
    }
}

void Global::loadStreetStructures(){ // Loads streetSegmentInfo & streetStreetSegment & streetIntersection  & streetSegmentTravelTime:
    int numberOfSegments = getNumStreetSegments();
    streetIntersection.resize(getNumStreets());
    //Resize to the number of streets
    streetStreetSegment.resize(getNumStreets());
    for(int i=0; i<numberOfSegments ; i++){ //We go through all segments and add them to the corresponding street
        InfoStreetSegment segInfo = getInfoStreetSegment(i);    
        //streetSegmentInfo:                    //Also we find the information and store it for -
        streetSegmentInfo.push_back(segInfo);   //for all of our street segments
        //streetStreetSegment:
        streetStreetSegment[segInfo.streetID].push_back(i);

        //streetIntersection:
        streetIntersection[segInfo.streetID].insert(segInfo.from);
        streetIntersection[segInfo.streetID].insert(segInfo.to);

        //streetSegmentTravelTime:
        streetSegmentTravelTime.push_back(find_street_segment_length(i)/(segInfo.speedLimit * KM_H_TO_M_S));
    
        maxSpeedLimit = std::max(maxSpeedLimit,segInfo.speedLimit);
    }
}

void Global::loadStreetNameDictionary(){
    for(int i=0; i<getNumStreets(); i++){
        string name = getStreetName(i);
        transform(name.begin(), name.end(), name.begin(),::tolower);
        int index1 = charToIndex(name[0]);
        int index2 = charToIndex(name[1]);
        streetNameDictionary[index1][index2].insert(std::pair<std::string, unsigned> (name, i));
    }
}

void Global::loadStreetOSMData(){
    for(unsigned i = 0; i<getNumberOfWays(); i++){
        const OSMWay* osmway = getWayByIndex(i);
        OSMID osmid = osmway->id();
        for(unsigned j = 0; j<getTagCount(osmway); j++){
            std::pair<std::string, std::string> tag = getTagPair(osmway,j);
            if(tag.first == "highway")
                streetOSMData.insert(std::pair<OSMID,std::pair<std::string, std::string>> (osmid, tag));
        }
    }
}

void Global::loadRailwayOSMData(){
    for(unsigned i = 0; i<getNumberOfNodes(); i++){
        const OSMNode* osmnode = getNodeByIndex(i);
        std::string name;
        std::string operatorName;
        LatLon position = osmnode->coords();
        bool isSubwayStation = false;
        for(unsigned j = 0; j<getTagCount(osmnode); j++){
            std::pair<std::string, std::string> tag = getTagPair(osmnode,j);
            if(tag.first == "name")
                name = tag.second;
            else if(tag.first == "operator")
                operatorName = tag.second;
            else if((tag.first == "railway") && (tag.second == "station"))
                isSubwayStation = true;
        }
        std::pair<std::string, std::string> pairName (name, operatorName);
        if(isSubwayStation)
            subwayStationOSMData.push_back(std::pair<std::pair<std::string, std::string>, LatLon> (pairName,position));
    }
}

void Global::loadHotelsOSMData(){
    // Look through all OSMnodes and look at their tags to find which are hotels
    for(unsigned i = 0; i < getNumberOfNodes(); i++){
        const OSMNode* osmnode = getNodeByIndex(i);
        std::string name;
        LatLon position = osmnode->coords();
        bool isHotel = false;
        for(unsigned j = 0; j < getTagCount(osmnode); j++){
            std::pair<std::string, std::string> tag = getTagPair(osmnode,j);
            if(tag.first == "name")
                name = tag.second;
            else if((tag.first == "building" || tag.first == "tourism") && (tag.second == "hotel")){
                isHotel = true;
            }
        }
        //If hotel found, push in vector
        std::string hotelName = name;
        if(isHotel)
            hotelsOSMData.push_back(std::pair<std::string, LatLon>(hotelName, position));
            
    } 
}

//Load weather data from DarkSky API
//Credits: www.eecg.utoronto.ca/~vaughn/ece297/ECE297/assignments/libcurl/libcurl.pdf
void Global::loadWeatherData(){
    //Get lat lon of city that is loaded
    std::string latStr= cityLatLonData.find(cityName)->second.first;
    std::string lonStr= cityLatLonData.find(cityName)->second.second;
    char lat[10]; //size 9 because all lat lons have been set to 9 digits
    char lon[10];
    strcpy(lat, latStr.c_str());
    strcpy(lon, lonStr.c_str());
    
    CURLcode res;
    CURL *curlHandle = curl_easy_init();
    if ( !curlHandle ) {
        cout << "ERROR: Unable to get easy handle" << endl;
        return;
    } else {
        char errbuf[CURL_ERROR_SIZE] = {0};
        dataStruct myStruct;
        char targetURL[] = "https://api.darksky.net/forecast/ba334561f000ca2ccbe6b29aac0e31a7/";
        //concatenate lat lon to URL path to get weather data
        strcat(targetURL, lat);
        strcat(targetURL, ",");
        strcat(targetURL, lon);

        res = curl_easy_setopt(curlHandle, CURLOPT_URL, targetURL);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ERRORBUFFER, errbuf);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, write_data);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &myStruct);
        if (res == CURLE_OK)
            res = curl_easy_setopt(curlHandle, CURLOPT_ACCEPT_ENCODING, "");

        myStruct.url = targetURL;

        if (res != CURLE_OK) {
            cout << "ERROR: Unable to set libcurl option" << endl;
            cout << curl_easy_strerror(res) << endl;
        } else {
            res = curl_easy_perform(curlHandle);
        }

        if (res == CURLE_OK) {
            // Create an empty proper tree
            ptree ptRoot;
            /* Store JSON data into a Property Tree
             *
             * read_json() expects the first parameter to be an istream object,
             * or derived from istream (e.g. ifstream, istringstream, etc.).
             * The second parameter is an empty property tree.
             *
             * If your JSON data is in C-string or C++ string object, you can
             * pass it to the constructor of an istringstream object.
             */
            istringstream issJsonData(myStruct.response);
            read_json(issJsonData, ptRoot);

            // Parsing and printing the data
            try {
                getWeatherData(ptRoot);
            } catch (const char *errMsg) {
                cout << "ERROR: Unable to fully parse the weather dark sky api data" << endl;
                cout << "Thrown message: " << errMsg << endl;
            }
        } else {
            cout << "ERROR: res == " << res << endl;
            cout << errbuf << endl;
        }

        if (myStruct.response)
            delete []myStruct.response;

        curl_easy_cleanup(curlHandle);
        curlHandle = nullptr;
    }
}

void Global::loadCityLatLonData(){
    //Store each city with the latitude and longitude of the city
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("beijing", std::pair<string, string> ("39.90420", "116.4074")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("cairo", std::pair<string, string> ("30.04442", "31.23570")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("cape-town", std::pair<string, string> ("33.92490", "18.42410")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("golden-horseshoe", std::pair<string, string> ("43.60000", "-79.73000")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("hamilton", std::pair<string, string> ("43.25570", "-79.87110")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("hong-kong", std::pair<string, string> ("22.39640", "114.1095")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("iceland", std::pair<string, string> ("64.96310", "64.96310")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("interlaken", std::pair<string, string> ("46.68630", "7.863000")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("london", std::pair<string, string> ("51.50742", "-0.12780")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("moscow", std::pair<string, string> ("55.75580", "37.61730")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("new-delhi", std::pair<string, string> ("28.61390", "77.20900")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("new-york", std::pair<string, string> ("40.71280", "-74.0060")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("rio-de-janeiro", std::pair<string, string> ("-22.9068", "-43.1729")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("saint-helena", std::pair<string, string> ("-15.9650", "-5.70890")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("singapore", std::pair<string, string> ("1.352100", "103.8198")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("sydney", std::pair<string, string> ("-33.8688", "151.2093")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("tehran", std::pair<string, string> ("35.68920", "51.38900")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("tokyo", std::pair<string, string> ("35.68950", "139.6917")));
    cityLatLonData.insert(std::pair<string, std::pair<string, string>> ("toronto", std::pair<string, string> ("43.65107", "-79.3470")));
}

void Global::getWeatherData(ptree &ptRoot){
    //get summary, apparentTemp and windSpeed data and put it in weatherData
    weatherData.push_back(ptRoot.get<string>("currently.summary"));
    weatherData.push_back(ptRoot.get<string>("currently.apparentTemperature"));
    weatherData.push_back(ptRoot.get<string>("currently.windSpeed"));
}

//get data from API
//credits: www.eecg.utoronto.ca/~vaughn/ece297/ECE297/assignments/libcurl/libcurl.pdf
size_t write_data(void *buffer, size_t, size_t nmemb, void *userp){
    if (buffer && nmemb && userp) {
        dataStruct *pMyStruct = (dataStruct *)userp;

        // Writes to struct passed in from main
        if (pMyStruct->response == nullptr) {
            // Case when first time write_data() is invoked
            pMyStruct->response = new char[nmemb + 1];
            strncpy(pMyStruct->response, (char *)buffer, nmemb);
        }
        else {
            // Case when second or subsequent time write_data() is invoked
            char *oldResp = pMyStruct->response;

            pMyStruct->response = new char[pMyStruct->size + nmemb + 1];

            // Copy old data
            strncpy(pMyStruct->response, oldResp, pMyStruct->size);

            // Append new data
            strncpy(pMyStruct->response + pMyStruct->size, (char *)buffer, nmemb);

            delete []oldResp;
        }

        pMyStruct->size += nmemb;
        pMyStruct->response[pMyStruct->size] = '\0';
    }

    return nmemb;
}

void Global::clearDictionary(){
    for(int i=0; i<DICTIONARY_SIZE; i++){
        for(int j=0; j<DICTIONARY_SIZE; j++){
             streetNameDictionary[i][j].clear();  
        }
    }
}

//Convert character into index of vector
int charToIndex(char c){
    if('A'<c && c<'Z'){return c-'A';}
    if('a'<c && c<'z'){return c-'a';}
    return 'z'-'a'+1;//Last room for non-alphabetical
}
