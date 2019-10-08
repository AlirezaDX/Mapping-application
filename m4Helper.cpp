#include "m4Helper.h"


DeliveryTracking::DeliveryTracking(const DeliveryRouting& delData,
                    const std::vector<unsigned>& depots)
                    : deliveriesList(*delData.deliveriesPtr){
    
    delDataPtr = &delData;
    const std::vector<DeliveryInfo>& deliveries = *delData.deliveriesPtr;
    deliveryState.resize(deliveries.size(), DELIVERY_STATE_AVAILABLE);
    intersectionsInfo.resize(delData.getNumInvolvedIntersections());
    for(unsigned i=0; i<deliveries.size();i++){ //Add pickUps &  dropOffs to involved intersections
        const unsigned curPickInter = deliveries[i].pickUp;
        //Find the index in which the currPickInter corresponds to and add the deliveryID for it's pick vector
        intersectionsInfo[delData.getInvolvedInterIndex(curPickInter)].pickDeliveryIDs.push_back(i);
        intersectionsInfo[delData.getInvolvedInterIndex(curPickInter)].isDepot = false;
                
        const unsigned curDropInter = deliveries[i].dropOff;
        //Find the index in which the currDropInter corresponds to and add the deliveryID for it's drop vector
        intersectionsInfo[delData.getInvolvedInterIndex(curDropInter)].dropDeliveryIDs.push_back(i);
        intersectionsInfo[delData.getInvolvedInterIndex(curDropInter)].isDepot = false;
    }
    for(unsigned i=0; i<depots.size();i++){ //Add depots to involved intersections
        const unsigned curInter = depots[i];
            intersectionsInfo[delData.getInvolvedInterIndex(curInter)].isDepot = true;     
    }
}

DeliveryTracking::DeliveryTracking(const DeliveryRouting& delData, const std::vector<mySubpath> mySP,
                    const std::vector<unsigned>& depots)
                    : deliveriesList(*delData.deliveriesPtr){
    delDataPtr = &delData;
    const std::vector<DeliveryInfo>& deliveries = *delData.deliveriesPtr;
    deliveryState.resize(deliveries.size(), DELIVERY_STATE_AVAILABLE);
    intersectionsInfo.resize(delData.getNumInvolvedIntersections());
    for(unsigned i=0; i<deliveries.size();i++){ //Add pickUps &  dropOffs to involved intersections
        const unsigned curPickInter = deliveries[i].pickUp;
        //Find the index in which the currPickInter corresponds to and add the deliveryID for it's pick vector
        intersectionsInfo[delData.getInvolvedInterIndex(curPickInter)].pickDeliveryIDs.push_back(i);
        intersectionsInfo[delData.getInvolvedInterIndex(curPickInter)].isDepot = false;
                
        const unsigned curDropInter = deliveries[i].dropOff;
        //Find the index in which the currDropInter corresponds to and add the deliveryID for it's drop vector
        intersectionsInfo[delData.getInvolvedInterIndex(curDropInter)].dropDeliveryIDs.push_back(i);
        intersectionsInfo[delData.getInvolvedInterIndex(curDropInter)].isDepot = false;
    }
    for(unsigned i=0; i<depots.size();i++){ //Add depots to involved intersections
        const unsigned curInter = depots[i];
            intersectionsInfo[delData.getInvolvedInterIndex(curInter)].isDepot = true;     
    }
    for(unsigned i = 0; i<mySP.size(); i++){
        for(unsigned j=0; j<mySP[i].pickUp_indices.size(); j++){
            pickUp(mySP[i].pickUp_indices[j]);
        }
        for(unsigned j=0; j<mySP[i].dropOff_indices.size(); j++){
            dropOff(mySP[i].dropOff_indices[j]);
        }
    }
}

DeliveryTracking::DeliveryTracking(const DeliveryRouting& delData, const std::vector<mySubpath> mySP, unsigned upToIndex,
                    const std::vector<unsigned>& depots)
                    : deliveriesList(*delData.deliveriesPtr){
    delDataPtr = &delData;
    const std::vector<DeliveryInfo>& deliveries = *delData.deliveriesPtr;
    deliveryState.resize(deliveries.size(), DELIVERY_STATE_AVAILABLE);
    intersectionsInfo.resize(delData.getNumInvolvedIntersections());
    for(unsigned i=0; i<deliveries.size();i++){ //Add pickUps &  dropOffs to involved intersections
        const unsigned curPickInter = deliveries[i].pickUp;
        //Find the index in which the currPickInter corresponds to and add the deliveryID for it's pick vector
        intersectionsInfo[delData.getInvolvedInterIndex(curPickInter)].pickDeliveryIDs.push_back(i);
        intersectionsInfo[delData.getInvolvedInterIndex(curPickInter)].isDepot = false;
                
        const unsigned curDropInter = deliveries[i].dropOff;
        //Find the index in which the currDropInter corresponds to and add the deliveryID for it's drop vector
        intersectionsInfo[delData.getInvolvedInterIndex(curDropInter)].dropDeliveryIDs.push_back(i);
        intersectionsInfo[delData.getInvolvedInterIndex(curDropInter)].isDepot = false;
    }
    for(unsigned i=0; i<depots.size();i++){ //Add depots to involved intersections
        const unsigned curInter = depots[i];
            intersectionsInfo[delData.getInvolvedInterIndex(curInter)].isDepot = true;     
    }
    for(unsigned i = 0; i<upToIndex; i++){
        for(unsigned j=0; j<mySP[i].pickUp_indices.size(); j++){
            pickUp(mySP[i].pickUp_indices[j]);
        }
        for(unsigned j=0; j<mySP[i].dropOff_indices.size(); j++){
            dropOff(mySP[i].dropOff_indices[j]);
        }
    }
}

const DR_IntersectionInfo& DeliveryTracking::getIntersectionDeliveryInfo(int intersectionID) const{
    return intersectionsInfo[delDataPtr->getInvolvedInterIndex(intersectionID)];
}

void DeliveryTracking::pickUp(unsigned deliveryID){
    if( deliveryState[deliveryID] != DELIVERY_STATE_AVAILABLE) cout << "Error: picked" << endl;
    else{ //Debug
    deliveryState[deliveryID] = DELIVERY_STATE_PICKED;
    deliveriesList.pickedUp(deliveryID);
    }
}
void DeliveryTracking::dropOff(unsigned deliveryID){
    if( deliveryState[deliveryID] != DELIVERY_STATE_PICKED) cout << "Error: dropped" << endl;
    else{ //Debug
    deliveryState[deliveryID] = DELIVERY_STATE_DROPPED;
    deliveriesList.droppedOff(deliveryID);
    }
}

void DeliveryTracking::pickUp(unsigned deliveryID, mySubpath& currSP){
    if( deliveryState[deliveryID] != DELIVERY_STATE_AVAILABLE) cout << "Error: picked" << endl;
    else{ //Debug
    deliveryState[deliveryID] = DELIVERY_STATE_PICKED;
    deliveriesList.pickedUp(deliveryID);
    currSP.pickUp_indices.push_back(deliveryID);
    currSP.truck_capacity += (*delDataPtr->deliveriesPtr)[deliveryID].itemWeight;
    }
}
void DeliveryTracking::dropOff(unsigned deliveryID, mySubpath& currSP){
    if( deliveryState[deliveryID] != DELIVERY_STATE_PICKED) cout << "Error: dropped" << endl;
    else{ //Debug
    deliveryState[deliveryID] = DELIVERY_STATE_DROPPED;
    deliveriesList.droppedOff(deliveryID);
    currSP.dropOff_indices.push_back(deliveryID);
    currSP.truck_capacity -= (*delDataPtr->deliveriesPtr)[deliveryID].itemWeight;
    }
}

bool DeliveryTracking::isAvailable(unsigned deliveryID) const{
    return deliveryState[deliveryID] == DELIVERY_STATE_AVAILABLE;
}
bool DeliveryTracking::isPicked(unsigned deliveryID) const{
    return deliveryState[deliveryID] == DELIVERY_STATE_PICKED;
}
bool DeliveryTracking::isDropped(unsigned deliveryID) const{
    return deliveryState[deliveryID] == DELIVERY_STATE_DROPPED;
}

unsigned DeliveryTracking::getDeliveryState(unsigned deliveryID) const{
    return deliveryState[deliveryID];
}


DeliveryRouting::DeliveryRouting(const std::vector<DeliveryInfo>& deliveries,
                        const std::vector<unsigned>& depots,
                        const float right_turn_penalty,
                        const float left_turn_penalty)
                        : deliveriesList(deliveries){
    
//    auto const start = std::chrono::high_resolution_clock::now();
    
    deliveriesPtr = &deliveries;
    leftTurnPenalty = left_turn_penalty;
    rightTurnPenalty = right_turn_penalty;
    
    //Resize the vector to hold the indices of the 2D vector-intersectionsInvolved-. 
    //Default -1 for those not involved.
    intersectionsInvolvedIndex.resize(getNumIntersections(), -1);
    //Overestimate the size of the involved intersections for efficiency
    intersectionsInvolved.reserve(deliveries.size()*9/10+depots.size());
    numIntersInvolved = 0;
    
    for(unsigned i=0; i<deliveries.size();i++){ //Add pickUps &  dropOffs to involved intersections
        const unsigned& curPickInter = deliveries[i].pickUp;
        if(intersectionsInvolvedIndex[curPickInter] == -1){ //If unique
            //Push back and store the index
            intersectionsInvolved.push_back(curPickInter);
            intersectionsInvolvedIndex[curPickInter] = numIntersInvolved;
            numIntersInvolved++;
        }
        
        const unsigned& curDropInter = deliveries[i].dropOff;
        if(intersectionsInvolvedIndex[curDropInter] == -1){ //If unique
            //Push back and store the index
            intersectionsInvolved.push_back(curDropInter);
            intersectionsInvolvedIndex[curDropInter] = numIntersInvolved;
            numIntersInvolved++;
        }
        
    }
    for(unsigned i=0; i<depots.size();i++){ //Add depots to involved intersections
        const unsigned& curInter = depots[i];
        if(intersectionsInvolvedIndex[curInter] == -1){ //Making sure it was not added before
            //Push back and store the index of the intersections position
            intersectionsInvolved.push_back(curInter);
            intersectionsInvolvedIndex[curInter] = numIntersInvolved;
            numIntersInvolved++;
        }    
    }
    
    //Resize the 2d vector to the number of involved intersections
    computedPaths.resize(numIntersInvolved, std::vector<foundPath>(numIntersInvolved));
    sortedComputedPathsIndex.resize(numIntersInvolved, std::vector<size_t>(numIntersInvolved));
    //Find and store the path from the intersection to all intersections involved for all involved
    #pragma omp parallel for
    for(unsigned i=0; i<numIntersInvolved; i++){
        const int& currInvolvedInter = intersectionsInvolved[i];
        computedPaths[i] = findPathMultiDest(currInvolvedInter, intersectionsInvolved,
                            rightTurnPenalty, leftTurnPenalty);
        const std::vector<foundPath>& currFoundPath = computedPaths[i];
        //Fills the vector 0,1,2,3,...:
        std::iota(sortedComputedPathsIndex[i].begin(), sortedComputedPathsIndex[i].end(), 0);
        //Sorts the indices to correspond to how computedPaths[i] would be sorted.
        std::sort(sortedComputedPathsIndex[i].begin(), sortedComputedPathsIndex[i].end(),
                   [&currFoundPath](size_t i1, size_t i2) {return currFoundPath[i1].timeToTravel < currFoundPath[i2].timeToTravel;}); 
    }
//    auto const end = std::chrono::high_resolution_clock::now();
//    auto const delta = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
//    std::cout << "Time to create DeliveryRouting: " <<delta.count() << endl;
}

int DeliveryRouting::getNumInvolvedIntersections()const{
    return numIntersInvolved;
}

const foundPath& DeliveryRouting::getFoundPath(int fromInterID, int toInterID)const{
    //if(fromFPID==-1 || toFPID==-1){
    //    cout << "Error getFoundPath:Input not and involved intersection" << endl;
    //}
    return computedPaths[intersectionsInvolvedIndex[fromInterID]/*fromFPID*/][intersectionsInvolvedIndex[toInterID]/*toFPID*/];
}

double DeliveryRouting::getTime(int fromInterID, int toInterID)const{
    /*const foundPath& currFP = getFoundPath(fromInterID, toInterID);
    return currFP.timeToTravel;*/
    return computedPaths[intersectionsInvolvedIndex[fromInterID]/*fromFPID*/][intersectionsInvolvedIndex[toInterID]/*toFPID*/].timeToTravel;
}

double DeliveryRouting::getTime(int fromInterID, int toInterID, int prevSSID)const{
    const foundPath& currFP =getFoundPath(fromInterID, toInterID);
    //Consider turn penalty
    if(_DATA.streetSegmentInfo[prevSSID].streetID != _DATA.streetSegmentInfo[currFP.path[0]].streetID){
        return currFP.timeToTravel + rightTurnPenalty;
    }
    return currFP.timeToTravel;
}

double DeliveryRouting::getTurnTimeOffset(int prevSSID, int nextSSID)const{
    if(_DATA.streetSegmentInfo[prevSSID].streetID != _DATA.streetSegmentInfo[nextSSID].streetID)
        return rightTurnPenalty;
    return 0;
}

double DeliveryRouting::getTurnTimeOffset(int fromInterID, int toInterID, int prevSSID)const{
    /*double pathFirstSS = getFoundPath(fromInterID, toInterID).path[0];
    if(_DATA.streetSegmentInfo[prevSSID].streetID != _DATA.streetSegmentInfo[pathFirstSS].streetID){
        return rightTurnPenalty;
    }
    return 0; */
    //Consider turn penalty
    return getTurnTimeOffset(prevSSID, computedPaths[intersectionsInvolvedIndex[fromInterID]][intersectionsInvolvedIndex[toInterID]].path[0]);
}

const foundPath& DeliveryRouting::getFasterFoundPath(int fromInterID, int index) const{
    index+=1; //Since sorted in ascending order first is the same intersection which we don't want to return.
    //make sure index is not larger than size of array
    if(index >= int(sortedComputedPathsIndex[intersectionsInvolvedIndex[fromInterID]].size()))
        index = sortedComputedPathsIndex[intersectionsInvolvedIndex[fromInterID]].size() - 1;
    return computedPaths[intersectionsInvolvedIndex[fromInterID]][sortedComputedPathsIndex[intersectionsInvolvedIndex[fromInterID]][index]];
}

double DeliveryRouting::getFasterTime(int fromInterID, int index) const{
    index+=1; //Since sorted in ascending order first is the same intersection which we don't want to return.
    //make sure index is not larger than size of array
    if(index >= int(sortedComputedPathsIndex[intersectionsInvolvedIndex[fromInterID]].size()))
        index = sortedComputedPathsIndex[intersectionsInvolvedIndex[fromInterID]].size() - 1;
    return computedPaths[intersectionsInvolvedIndex[fromInterID]][sortedComputedPathsIndex[intersectionsInvolvedIndex[fromInterID]][index]].timeToTravel;
}

double DeliveryRouting::getFasterTime(int fromInterID, int prevSSID,
                                        int index) const{
    index+=1; //Since sorted in ascending order first is the same intersection.
    //make sure index is not larger than size of array
    if(index >= int(sortedComputedPathsIndex[intersectionsInvolvedIndex[fromInterID]].size()))
        index = sortedComputedPathsIndex[intersectionsInvolvedIndex[fromInterID]].size() - 1;
    const foundPath& currFP = computedPaths[intersectionsInvolvedIndex[fromInterID]][sortedComputedPathsIndex[fromInterID][index]];
    if(_DATA.streetSegmentInfo[prevSSID].streetID != _DATA.streetSegmentInfo[currFP.path[0]].streetID){
        return currFP.timeToTravel + rightTurnPenalty;
    }
    return currFP.timeToTravel;
}


//constructor for delivery list
DeliveriesList::DeliveriesList(const std::vector<DeliveryInfo>& deliveries){
    //add each delivery index into list
    for(unsigned i=0; i<deliveries.size(); i++){
        availablePickUps.push_back(i);
    }
    
    //set available pick up iterator to first item in list
    availablePickUpsIt = availablePickUps.begin();
    
    //set available drop off iterator to end of list
    //no drop offs available yet
    availableDropOffsIt = (std::list<int>::iterator) nullptr;
    
    //set number of available pick ups and drop offs
    numOfPickUps = int(availablePickUps.size());
    numOfDropOffs = 0;
}

//destructor for delivery list
DeliveriesList::~DeliveriesList(){
    availablePickUps.clear();
    availableDropOffs.clear();
}

//get iterator of first pick up
std::list<int>::iterator DeliveriesList::getPickUp(){
    return availablePickUpsIt;
}

//get iterator of first drop off
std::list<int>::iterator DeliveriesList::getDropOff(){
    return availableDropOffsIt;
}

//get number of available pick ups
int DeliveriesList::getNumOfPickUps(){
    return numOfPickUps;
}

//get number of available drop offs
int DeliveriesList::getNumOfDropOffs(){
    return numOfDropOffs;
}

//get if everything has been picked up
bool DeliveriesList::finishedPickUps(){
    if(numOfPickUps == 0)
        return true;
    else 
        return false;
}

//get if everything has been dropped off
bool DeliveriesList::finishedDropOffs(){
    if(numOfDropOffs == 0)
        return true;
    else 
        return false;
}

//package has been picked up
//update available pick up and available drop off list
void DeliveriesList::pickedUp(int packageID){
    std::list<int>::iterator it = availablePickUpsIt;
    
    //start from first pickup, looks through all pickup for one to erase
    for(int i=0; i<numOfPickUps; i++){
        //if p off package found, erase it
        if(*it == packageID){
            //if picking up first item in pick up list
            //update pick up iterator to next one in list
            if(it == availablePickUpsIt)
                availablePickUpsIt = availablePickUps.erase(it);
            else
                availablePickUps.erase(it);
            
            //reduce number of pick ups left
            numOfPickUps--;
            
            //package has been picked up, add it to avaliable drop off list
            availableDropOffs.push_back(packageID);
            
            //increase number of drop offs available
            numOfDropOffs++;
            
            //change drop off iterator to point to first element
            availableDropOffsIt = availableDropOffs.begin();
            
            return;
        }
        //check next package in list
        it++;
    }
}

//package has been dropped off
//update available drop offs list
void DeliveriesList::droppedOff(int packageID){
    std::list<int>::iterator it = availableDropOffsIt;
    
    //start from first drop off, looks through all drop offs for one to erase
    for(int i=0; i<numOfDropOffs; i++){
        //if dropped off package found, erase it
        if(*it == packageID){
            //if dropping off first item in drop off list
            //update drop off iterator to next one in list
            if(it == availableDropOffsIt)
                availableDropOffsIt = availableDropOffs.erase(it);
            else
                availableDropOffs.erase(it);
            
            //reduce number of drop offs left by one
            numOfDropOffs--;
            
            return;
        }
        //check next package in list
        it++;
    }
}

int checkFasterToGoTo(unsigned j, unsigned currInt, const DeliveryRouting& delData, const DeliveryTracking& trackData, float truck_capacity, float truck_load ){
    unsigned toInt;
    int delID;
    const foundPath* currFPPtr;
    foundPath empty;
    toInt = delData.getFasterFoundPath(currInt, j).toInterID;

    if(!trackData.getIntersectionDeliveryInfo(toInt).isDepot){

        if(trackData.getIntersectionDeliveryInfo(toInt).dropDeliveryIDs.size() > 0){
            //drop off every package possible
            for(unsigned k = 0; k < trackData.getIntersectionDeliveryInfo(toInt).dropDeliveryIDs.size(); k++){

                delID = trackData.getIntersectionDeliveryInfo(toInt).dropDeliveryIDs[k];

                currFPPtr = &delData.getFoundPath(currInt, (*delData.deliveriesPtr)[delID].dropOff);

                if(trackData.isPicked(trackData.getIntersectionDeliveryInfo(toInt).dropDeliveryIDs[k]) &&
                        !currFPPtr->path.empty() ){
                    return delID;
                }
            }
        }

        if(trackData.getIntersectionDeliveryInfo(toInt).pickDeliveryIDs.size() > 0){
            //store the delivery delID of the pickup 
            //Loop through possible pick ups at current intersection:
            for(unsigned k = 0; k < trackData.getIntersectionDeliveryInfo(toInt).pickDeliveryIDs.size(); k++){
                delID = trackData.getIntersectionDeliveryInfo(toInt).pickDeliveryIDs[k];

                currFPPtr = &delData.getFoundPath(currInt, (*delData.deliveriesPtr)[delID].pickUp);

                //if the delivery is available for pickup 
                //and the package does not exceed the truck capacity
                if(trackData.isAvailable(delID) && !currFPPtr->path.empty()){
                    if(truck_load + (*delData.deliveriesPtr)[delID].itemWeight <= truck_capacity){
                        return delID;
                    }
                    if(truck_load + (*delData.deliveriesPtr)[delID].itemWeight > truck_capacity)
                        continue;
                }
            }
        }
    }
    return -1;
}   

                // <<<<<<<<<<<< 2-OPT perturbation >>>>>>>>>>>>

//Doesn't consider turn times between subPaths <=BUG=>
std::pair<std::vector<mySubpath>,double> createPerturbation(const std::vector<mySubpath>& baseSP, const DeliveryRouting& delData, unsigned id1, unsigned id2){
    //new courier sub-path
    int newSPSize = id2-id1 +1;
    std::vector<mySubpath> newSP(newSPSize);
    const std::vector<DeliveryInfo>& deliveries = *(delData.deliveriesPtr);
    double newTime =0;
    //To understand algo draw and follow:
    //Perturb the first path:
    newSP.front().start_intersection = baseSP[id1].start_intersection;
    newSP.front().end_intersection = baseSP[id2].start_intersection;
    newSP.front().pickUp_indices = baseSP[id1].pickUp_indices;
    newSP.front().dropOff_indices = baseSP[id1].dropOff_indices;
    const foundPath* newFoundPathPtr = &delData.getFoundPath(newSP.front().start_intersection, newSP.front().end_intersection);
    newSP.front().time_elapsed = newFoundPathPtr->timeToTravel + baseSP[id1-1].time_elapsed;
    newSP.front().truck_capacity = baseSP[id1].truck_capacity;
    newTime += newFoundPathPtr->timeToTravel;// <BUG>
    //Perturb the last path:
    newSP.back().start_intersection = baseSP[id1].end_intersection;
    newSP.back().end_intersection = baseSP[id2].end_intersection;
    newSP.back().pickUp_indices = baseSP[id1+1].pickUp_indices;
    newSP.back().dropOff_indices = baseSP[id1+1].dropOff_indices;
    newFoundPathPtr = &delData.getFoundPath(newSP.back().start_intersection, newSP.back().end_intersection);
    newSP.back().time_elapsed = newFoundPathPtr->timeToTravel;
    newTime += newFoundPathPtr->timeToTravel;// <BUG>
    //Perturb paths in between:
    for(unsigned i=id2-1, j = 1; i>id1; i--, j++){
        newSP[j].start_intersection = newSP[j-1].end_intersection;
        newSP[j].end_intersection = baseSP[i].start_intersection;
        newSP[j].pickUp_indices = baseSP[i+1].pickUp_indices;
        newSP[j].dropOff_indices = baseSP[i+1].dropOff_indices;
        newSP[j].truck_capacity = newSP[j-1].truck_capacity;
        for(unsigned k=0; k<newSP[j].dropOff_indices.size(); k++){
            newSP[j].truck_capacity -= deliveries[newSP[j].dropOff_indices[k]].itemWeight;
        }
        for(unsigned k=0; k<newSP[j].pickUp_indices.size(); k++){
            newSP[j].truck_capacity += deliveries[newSP[j].pickUp_indices[k]].itemWeight;
        }
        newFoundPathPtr = &delData.getFoundPath(newSP[j].start_intersection, newSP[j].end_intersection);
        newSP[j].time_elapsed = newFoundPathPtr->timeToTravel + newSP[j-1].time_elapsed;
        newTime += newFoundPathPtr->timeToTravel;// <=BUG=>
    }
    newSP.back().truck_capacity = (*(newSP.end()-2)).truck_capacity;
    for(unsigned k=0; k<newSP.back().dropOff_indices.size(); k++){
        newSP.back().truck_capacity -= deliveries[newSP.back().dropOff_indices[k]].itemWeight;
    }
    for(unsigned k=0; k<newSP.back().pickUp_indices.size(); k++){
        newSP.back().truck_capacity += deliveries[newSP.back().pickUp_indices[k]].itemWeight;
    }
    newSP.back().time_elapsed += (*(newSP.end()-2)).time_elapsed;
    
    //cout << newSP.back().time_elapsed - baseSP[id1-1].time_elapsed << " vs" << newTime << endl;
    
    return {newSP,newTime};
}

#define LEGALITY_DELIVERY_DEFAULT 0
#define LEGALITY_DELIVERY_SENSITIVE_NOTPICKED 1
#define LEGALITY_DELIVERY_SENSITIVE_PICKED 2
bool checkLegalilty(const std::vector<mySubpath>& baseSP,
        const std::vector<mySubpath>& newSP, unsigned id1, unsigned id2,
        const std::vector<DeliveryInfo>& deliveries,
        const float truck_capacity){
    
    std::vector<unsigned> legalityStateDeliveries(deliveries.size(), LEGALITY_DELIVERY_DEFAULT);
    for(unsigned i=id1+1; i<=id2;i++){
        for(unsigned j=0; j<baseSP[i].pickUp_indices.size(); j++){
            unsigned curPick = baseSP[i].pickUp_indices[j];
            //Pick ups happening in perturbed area are sensitive and must be picked before dropped.
            legalityStateDeliveries[curPick] = LEGALITY_DELIVERY_SENSITIVE_NOTPICKED;
        }
    }
    float curCapacity = baseSP[id1].truck_capacity;
    for(unsigned j=1, i=id2; j<newSP.size(); j++, i--){
        //Check for weight
        //Check the sensitive list for dropOff coming before delivery
        //Update sensitivity picked up.
        for(unsigned k=0; k<newSP[j].pickUp_indices.size(); k++){
            unsigned curPick = newSP[j].pickUp_indices[k];
            curCapacity += deliveries[curPick].itemWeight;
            //Picked a sensitive delivery, fine if we drop it now:
            legalityStateDeliveries[curPick] = LEGALITY_DELIVERY_SENSITIVE_PICKED;
        }
        
        for(unsigned k=0; k<baseSP[i].dropOff_indices.size(); k++){
            unsigned curDrop = baseSP[i].dropOff_indices[k];
            //If we drop a sensitive delivery before picking it up:
            if(legalityStateDeliveries[curDrop] == LEGALITY_DELIVERY_SENSITIVE_NOTPICKED){
                //cout<< "Dropped before picked." << j << endl;
                return false;
            }
            curCapacity -= deliveries[curDrop].itemWeight;
        }
        //if(curCapacity != newSP[j].truck_capacity)
        //  cout<< "Unexpected result checkLegality." << curCapacity << " vs "<< newSP[j].truck_capacity << endl;
        if(curCapacity>truck_capacity){
            //cout<< "Weight exceeded." << endl;
            return false;
        }
    }
    if(curCapacity != newSP.back().truck_capacity)
        cout<< "Unexpected result checkLegality." << endl;
    return true;
}

bool applyTimedPerturbation(std::vector<mySubpath>& baseSP,
        unsigned id1, unsigned id2, double maxTimeOffset, const DeliveryRouting& delData,
        const float truck_capacity){
    std::pair<std::vector<mySubpath>,double> pertPathTime = createPerturbation(baseSP, delData, id1, id2);
    //***********Find the travel time for the baseSubPath including the turn time before and after perturbation
    /*
    double baseTime = 0;
    unsigned prevSS;
    const foundPath& firstFP = delData.getFoundPath(baseSP[id1].start_intersection, baseSP[id1].end_intersection);
    
    //Include the turn time to enter the pertArea
    prevSS = baseSP[id1-1].subpath.back();
    baseTime += delData.getTurnTimeOffset(prevSS, firstFP.path.front());
    //The time to travel firstFP
    baseTime += firstFP.timeToTravel;
    prevSS = firstFP.path.back();
    for(unsigned i=id1+1; i<id2; i++){
        const foundPath& currFP = delData.getFoundPath(baseSP[i].start_intersection, baseSP[i].end_intersection);
        baseTime += delData.getTurnTimeOffset(prevSS, currFP.path.front());
        //baseTime += currFP.timeToTravel;// <=BUG=> The turn time in perturbation area is not computed yet.
        prevSS = currFP.path.back();
    }
    const foundPath& lastFP = delData.getFoundPath(baseSP[id2].start_intersection, baseSP[id2].end_intersection);
    baseTime += delData.getTurnTimeOffset(prevSS, lastFP.path.front());
    baseTime += lastFP.timeToTravel;
    //Include the turn time to leave the pertArea
    prevSS = lastFP.path.back();
    baseTime += delData.getTurnTimeOffset(prevSS, baseSP[id2+1].subpath.front());
    
    //Done with finding baseTime, have to add turn times to the pertPathTime:
    double newTime = pertPathTime.second;
    newTime += delData.getTurnTimeOffset(baseSP[id1-1].subpath.back(), pertPathTime.first.front().subpath.front());
    newTime += delData.getTurnTimeOffset(pertPathTime.first.back().subpath.back(),baseSP[id2+1].subpath.front());
    */
    double newTime = pertPathTime.second;
    double baseTime = baseSP[id2].time_elapsed - baseSP[id1-1].time_elapsed;
    double newOffset = newTime - baseTime;
    if(newOffset < maxTimeOffset){
        //cout << "Pert better time." << endl;
        if(checkLegalilty(baseSP, pertPathTime.first, id1, id2,
        *delData.deliveriesPtr, truck_capacity)){
            //cout << "Pert legal." << endl;
            std::copy(pertPathTime.first.begin(), pertPathTime.first.end(), baseSP.begin()+id1);
            for(unsigned i=id2+1; i<baseSP.size(); i++){
                baseSP[i].time_elapsed += newOffset;
            }
            return true;
        }
    }
    return false;
}

        // <<<<<<<<<<<< Delivery perturbation >>>>>>>>>>>>

std::vector<mySubpath> perturbDeliveryOrder(std::vector<mySubpath>& path, 
                                           const std::vector<DeliveryInfo>& deliveries,
                                           const std::vector<unsigned>& depots,
                                           const DeliveryRouting& delData,
                                           const float truck_capacity){
    //create vector to hold delivery order
    //boolean will hold whether it is a pick-up (true) or drop-off(false)
    //unsigned for delivery index
    std::vector<std::pair<bool, unsigned>> deliveryOrder;
    std::vector<std::pair<bool, unsigned>> perturbedOrder;
    
    //get order of deliveries made in path
    for(unsigned i=0; i<path.size(); i++){
        for(unsigned j=0; j<path[i].pickUp_indices.size(); j++)
            deliveryOrder.push_back(std::pair<bool, unsigned> (true, path[i].pickUp_indices[j]));
        for(unsigned j=0; j<path[i].dropOff_indices.size(); j++)
            deliveryOrder.push_back(std::pair<bool, unsigned> (false, path[i].dropOff_indices[j]));
    }
    
    //fastest courier path and path time
    //exclude first path since we are not considering traveling from depot time
    std::vector<std::pair<bool, unsigned>> fastestOrder = deliveryOrder;
    double fastestTime = path.back().time_elapsed;
    unsigned startDepot = path[0].start_intersection;
    unsigned endDepot = path.back().end_intersection;
    
    perturbedOrder = deliveryOrder;
    //perturbedOrder now holds order deliveries are made
    //get iterators to point to element in vector
    std::vector<std::pair<bool, unsigned>>::iterator begin = perturbedOrder.begin();
    std::vector<std::pair<bool, unsigned>>::iterator end = perturbedOrder.end() - 1;
    
    while(end != begin){
        for(auto start = perturbedOrder.begin(); start != end; start++){
            //reverse order of part of vector
            std::reverse(start, end);

            //check legality of new vector
            bool legal = checkDeliveryOrder(perturbedOrder, deliveries, truck_capacity);
            
            //if a legal path is found, check if it is faster
            if(legal){
                //use fastestTime
                double perturbedTime = 0;
                for(unsigned i = 0; i<perturbedOrder.size()-1; i++){
                    unsigned startInter; 
                    unsigned endInter;
                    //check if start intersection is pick up or drop off
                    if(perturbedOrder[i].first)
                        startInter = deliveries[perturbedOrder[i].second].pickUp;
                    else
                        startInter = deliveries[perturbedOrder[i].second].dropOff;
                    
                    //check if end intersection is pick up or drop off
                    if(perturbedOrder[i+1].first)
                        endInter = deliveries[perturbedOrder[i+1].second].pickUp;
                    else
                        endInter = deliveries[perturbedOrder[i+1].second].dropOff;
                    
                    perturbedTime += delData.getTime(startInter, endInter);
                }
                
                unsigned startIntersection;
                unsigned endIntersection;
                unsigned depotStartTemp = depots[0];
                unsigned depotEndTemp = depots[0];
                
                //get whether first intersection is pick up or drop off
                if(perturbedOrder[0].first)
                    startIntersection = deliveries[perturbedOrder[0].second].pickUp;
                else 
                    startIntersection = deliveries[perturbedOrder[0].second].dropOff;
                
                //get whether last intersection is pick up or drop off
                if(perturbedOrder[perturbedOrder.size() - 1].first)
                    endIntersection = deliveries[perturbedOrder.back().second].pickUp;
                else 
                    endIntersection = deliveries[perturbedOrder.back().second].dropOff;
                
                //get time from first depot to first intersection
                double startClosestDepotTime = delData.getTime(startIntersection, depots[0]);
                double endClosestDepotTime = delData.getTime(endIntersection, depots[0]);
                
                //check if any other depots are closer
                for(unsigned i=1; i<depots.size(); i++){
                    //time from depot to first intersection
                    double startTime = delData.getTime(startIntersection, depots[i]);
                    //time from last intersection to depot
                    double endTime = delData.getTime(endIntersection, depots[i]);
                    //check if time is faster for start depot
                    if(startTime<startClosestDepotTime){
                        startClosestDepotTime = startTime;
                        //store closest depot for path reconstruction
                        depotStartTemp = depots[i];
                    }
                    //check if time is faster for end depot
                    if(endTime<endClosestDepotTime){
                        endClosestDepotTime = endTime;
                        //store closest depot for path reconstruction
                        depotEndTemp = depots[i];
                    }
                }
                
                //add time to total travel time
                perturbedTime += startClosestDepotTime;
                perturbedTime += endClosestDepotTime;
                
                //check if new perturbed path has a faster time
                if(perturbedTime < fastestTime){
                    //faster path found, store new fastest path
                    fastestTime = perturbedTime;
                    //clear old path and add new path
                    fastestOrder.clear();
                    fastestOrder = perturbedOrder;
                    //assign new fastest depots for reconstruction of path
                    startDepot = depotStartTemp;
                    endDepot = depotEndTemp;
                }
            }
            
            //reset vector to check new perturbation
            perturbedOrder = deliveryOrder;
        }
        //move back one element in list to find new perturbation
        end--;
    }
    
    std::vector<mySubpath> newFastPath;  
    double prevTruckWeight = 0;
    double elapsedTime = 0;
    //tried all perturbations, return fastest path found
    //recreate the vector of mySubPath
    
    //start with first depot
    newFastPath.emplace_back();
    //assign start and end intersections
    newFastPath.back().start_intersection = startDepot;
    newFastPath.back().end_intersection = deliveries[fastestOrder[0].second].pickUp;
    //set truck weight at start intersection
    newFastPath.back().truck_capacity = prevTruckWeight;
    //accumulate time and set time for sub-path
    elapsedTime += delData.getTime(newFastPath.back().start_intersection, newFastPath.back().end_intersection);
    newFastPath.back().time_elapsed = elapsedTime;
      
    //add all deliveries
    for(int i = 0; i<int(fastestOrder.size()-1); i++){
        //add new element to list
        newFastPath.emplace_back();
        
        //assign truck weight
        newFastPath.back().truck_capacity = prevTruckWeight;

        std::pair<bool, unsigned> curInfo = fastestOrder[i];
        std::pair<bool, unsigned> nextInfo = fastestOrder[i+1];
        
        //check if start intersection is pick up or drop off
        if(curInfo.first){
            //is a pick up
            newFastPath.back().start_intersection = deliveries[curInfo.second].pickUp;
            newFastPath.back().pickUp_indices.push_back(curInfo.second);
            newFastPath.back().truck_capacity += deliveries[curInfo.second].itemWeight;
        }
        else{
            //is a drop off
            newFastPath.back().start_intersection = deliveries[curInfo.second].dropOff;
            newFastPath.back().dropOff_indices.push_back(curInfo.second);
            newFastPath.back().truck_capacity -= deliveries[curInfo.second].itemWeight;
        }
        
        //check if end intersection is pick up or drop off
        if(nextInfo.first)
            //is a pick up
            newFastPath.back().end_intersection = deliveries[nextInfo.second].pickUp;
        else
            //is a drop off
            newFastPath.back().end_intersection = deliveries[nextInfo.second].dropOff;
        
        //start and end intersection has been determined and accounted for multiple package pick up and drop offs
        //update time elapsed for traveling current path
        elapsedTime += delData.getTime(newFastPath.back().start_intersection, newFastPath.back().end_intersection);
        //assign elapsed time for path
        newFastPath.back().time_elapsed = elapsedTime;
        
        //update truck weight for next delivery
        prevTruckWeight = newFastPath.back().truck_capacity;
    }
      
    //start intersection of new path is end of last path
    unsigned startInter = newFastPath.back().end_intersection;

    //add new element to path
    newFastPath.emplace_back();

    //set start and end intersection
    newFastPath.back().start_intersection = startInter;
    newFastPath.back().end_intersection = endDepot;

    //pick-up/drop-off last package at intersection
    if(fastestOrder.back().first){
        //is a pick up
        newFastPath.back().pickUp_indices.push_back(fastestOrder.back().second);
        newFastPath.back().truck_capacity += deliveries[fastestOrder.back().second].itemWeight;
    }
    else{
        //is a drop off
        newFastPath.back().dropOff_indices.push_back(fastestOrder.back().second);
        newFastPath.back().truck_capacity -= deliveries[fastestOrder.back().second].itemWeight;
    }

    //update time elapsed and assign elapsed time for path
    newFastPath.back().time_elapsed = elapsedTime + delData.getTime(newFastPath.back().start_intersection, newFastPath.back().end_intersection);
    
    return newFastPath;   
}

bool checkDeliveryOrder(std::vector<std::pair<bool, unsigned>>& path, const std::vector<DeliveryInfo>& deliveries, const float truck_capacity){
    //initialize a vector to hold whether a package has been picked up
    //not picked up == false, dropped off == true
    std::vector<int> deliveriesLegal (deliveries.size(), DELIVERY_STATE_AVAILABLE);
    float truckWeight = 0;
    
    for(unsigned i=0; i<path.size(); i++){
        //get first element of perturbed path
        std::pair<bool, unsigned> test = path[i];
        //if boolean of pair is true == pick up
        if(test.first){
            //checked if already picked up or dropped off
            if(deliveriesLegal[test.second] != DELIVERY_STATE_AVAILABLE){
                //if yes, trying to pick up package again 
                //or dropping off a package that's not picked up, illegal action
                return false;
            }
            //check if picking up package will overload truck
            if((truckWeight + deliveries[test.second].itemWeight) > truck_capacity)
                //if weight will be exceeded
                return false;
            //if not, set package to picked up
            deliveriesLegal[test.second] = DELIVERY_STATE_PICKED;
            //update truck weight
            truckWeight += deliveries[test.second].itemWeight;
        }
        //if boolean of pair is false == drop off
        else{
            //check if package has not been picked up
            if(deliveriesLegal[test.second] != DELIVERY_STATE_PICKED){
                //if package hasn't been picked up but trying to drop off, 
                //or dropping off package again (inefficient path)
                //path is illegal, return false
                return false;
            }
            //if dropping off a package will make the truck weight negative
            //somehow dropping off package that is picked up without adding weight to total
            if((truckWeight - deliveries[test.second].itemWeight) < -0.1){
                cout<<(truckWeight - deliveries[test.second].itemWeight)<<endl;
                return false;
            }
            deliveriesLegal[test.second] = DELIVERY_STATE_DROPPED;
            //update truck weight
            truckWeight -= deliveries[test.second].itemWeight;
        }   
    }
    
    return true;
}

std::vector<mySubpath> greedyPath(int depotIndex, const DeliveryRouting& delData, const std::vector<DeliveryInfo>& deliveries,
const std::vector<unsigned>& depots,
const float truck_capacity){
    
    DeliveryTracking trackData(delData, depots);
    //vector to hold total courier path
    vector<mySubpath> delPath;
    if(delData.getFasterFoundPath(depots[depotIndex], 0).path.empty()){
        return delPath; //EMPTY
    }
    delPath.reserve(deliveries.size()*2);
    //std::vector<CourierSubpath> courierPath;
    //courierPath.reserve(deliveries.size()*2);   
    
    double mintime = -1; 
    int startDelID = -1;
    unsigned del_inter;
    const foundPath* currFPPtr;
    unsigned prevSS;
    
    
    for(int i = 0; i < delData.getNumInvolvedIntersections(); i++){  //
        
        //find delivery locations from the first depot
        currFPPtr = &delData.getFasterFoundPath(depots[depotIndex], i);
        del_inter = currFPPtr->toInterID; 
        //if the location has at least 1 pick up, store the delivery ID
        if(trackData.getIntersectionDeliveryInfo(del_inter).pickDeliveryIDs.size() > 0){  
            startDelID = trackData.getIntersectionDeliveryInfo(del_inter).pickDeliveryIDs[0];
            break;
        }
    }
    
    //compare the rest of the depots with their nearby intersections

    delPath.push_back(mySubpath(depots[depotIndex], 0, currFPPtr->timeToTravel));
    //go from first depot to first pick up
    delPath.back().end_intersection = deliveries[startDelID].pickUp;
    //BUG more update required 
    
    prevSS = currFPPtr->path.back();
    
    delPath.push_back(mySubpath(currFPPtr->toInterID, delPath.back().truck_capacity, delPath.back().time_elapsed));

    //del_inter contains the to intersection from the depot
    //check if there are more pickups 
    for(unsigned i = 0; i < trackData.getIntersectionDeliveryInfo(del_inter).pickDeliveryIDs.size(); i++){
        //store the ID of the other pickup indices
        int pickIdx = trackData.getIntersectionDeliveryInfo(del_inter).pickDeliveryIDs[i];
        
        //if the package does not exceed the total truck capacity
        if(delPath.back().truck_capacity + deliveries[pickIdx].itemWeight <= truck_capacity){
            trackData.pickUp(pickIdx, delPath.back());
        }
    }
    
    
    while(!(trackData.deliveriesList.finishedDropOffs() && trackData.deliveriesList.finishedPickUps())){
     //Lets find the end_intersection of previous sub path:
        //find the nearest pickup
        unsigned fromInter = currFPPtr->toInterID;
        //int bestDelID;
        bool foundViablePath = false;
        // Secret Sauce:
        int j;
        for(j = 0; j < delData.getNumInvolvedIntersections() - 1; j++){
            int tempDelID =  checkFasterToGoTo(j, fromInter, delData, trackData, truck_capacity, delPath.back().truck_capacity);
            if( tempDelID != -1 ){
                //bestDelID = tempDelID;
                foundViablePath = true;//if a valid path was found break out
                break;   
            }
        }// Found the closest viable path.
        if(foundViablePath){
            bool foundBest = false;
            currFPPtr = &delData.getFasterFoundPath(fromInter, j);
            //Check for curved:
            double offSet = delData.getTurnTimeOffset(prevSS, currFPPtr->path.front());
            double bestTime = currFPPtr->timeToTravel + offSet;
            if(offSet != 0){
                while(!foundBest){
                    for(j=j+1 ; j < delData.getNumInvolvedIntersections() - 1; j++){
                        int tempDelID =  checkFasterToGoTo(j, fromInter, delData, trackData, truck_capacity, delPath.back().truck_capacity);
                        if( tempDelID != -1 ){
                            //bestDelID = tempDelID;
                            const foundPath& newFP = delData.getFasterFoundPath(fromInter, j);
                            if(newFP.timeToTravel > bestTime){
                                foundBest = true;
                                break;
                            }
                            double newOffSet = delData.getTurnTimeOffset(prevSS, newFP.path.front());
                            double newTime = newOffSet + newFP.timeToTravel;
                            if(newTime < bestTime){
                                bestTime = newTime;
                                currFPPtr = &newFP;
                                offSet = newOffSet; //doesn't matter
                            }
                        }
                    }
                    foundBest = true; //No more competitors 
                }
            }
            //We have the best time TOO.
            //We have the delivery
            //We have the path
            delPath.back().end_intersection = currFPPtr->toInterID;
            delPath.back().time_elapsed += bestTime;
            //delPath.back().subpath =  currFPPtr->path;
            prevSS = currFPPtr->path.back(); //Set the prevSS for the next itteration
            
        }
        //else //corner case if the pick up or delivery left is unreachable
          
        delPath.push_back(mySubpath(currFPPtr->toInterID, delPath.back().truck_capacity, delPath.back().time_elapsed));
        
        //Deliver and pick up:
        for(unsigned k = 0; k < trackData.getIntersectionDeliveryInfo(currFPPtr->toInterID).dropDeliveryIDs.size(); k++){//Go through the dropOff of intersection
            //Pick the pick ups:
            unsigned delID = trackData.getIntersectionDeliveryInfo(currFPPtr->toInterID).dropDeliveryIDs[k];
            if(trackData.isPicked(delID)){
                trackData.dropOff(delID, delPath.back());
            }
        }
        
        for(unsigned k = 0; k < trackData.getIntersectionDeliveryInfo(currFPPtr->toInterID).pickDeliveryIDs.size(); k++){//Go through the pickUps of intersection
            //Pick the pick ups:
            unsigned delID = trackData.getIntersectionDeliveryInfo(currFPPtr->toInterID).pickDeliveryIDs[k];
            if(trackData.isAvailable(delID)){
                if(delPath.back().truck_capacity + deliveries[delID].itemWeight <= truck_capacity){
                    trackData.pickUp(delID, delPath.back());
                }
            }
        }
        
        //cout<<"deliveries finished: " << delData.deliveriesList.finishedDropOffs() << endl;
    }
    
    //go to the nearest depot
    // the last bestDelID would be startDelID
    //we will be starting at a dropOff
    int depotID; //will store the ID of the closest depot
    int startIndex;
   
    for(unsigned k = 0; k < depots.size(); k++){//find first valid path to a depot assign it to min
        if(delData.getFoundPath(delPath.back().start_intersection, depots[k]).path.size() > 0){
            currFPPtr = &delData.getFoundPath(delPath.back().start_intersection, depots[k]);
            mintime = currFPPtr->timeToTravel;
            depotID = k;
            startIndex = k;
            break;
        }
    }
    for(unsigned j = startIndex; j < depots.size(); j++){
        currFPPtr = &delData.getFoundPath(delPath.back().start_intersection, depots[j]);
        if(!currFPPtr->path.empty()){
            if(currFPPtr->timeToTravel < mintime){

                mintime = currFPPtr->timeToTravel;
                depotID = j;
            }
        }
    }
    delPath.back().truck_capacity = 0;
    delPath.back().end_intersection = depots[depotID];
    delPath.back().time_elapsed += mintime;
    //delPath.back().subpath = delData.getFoundPath(delPath.back().start_intersection, delPath.back().end_intersection).path;
    
//    auto const end = std::chrono::high_resolution_clock::now();
//    auto const delta = std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
//    std::cout << "Time elapsed in traveling_courier: " <<delta.count() << endl;
    
    return delPath;
}

std::vector<CourierSubpath> getCourierPath(const std::vector<mySubpath>& finalAns, const DeliveryRouting& delData){
    std::vector<CourierSubpath> courierPath;
    //here we found the fastest path
    courierPath.emplace_back();
    //cout<<"courier index: "<<courierIndex<< " of size: "<< finalAns.size()<<endl;
    courierPath.back().start_intersection = finalAns[0].start_intersection;
    //cout<<"start intersection: " << courierPath.back().start_intersection<<endl;
    courierPath.back().pickUp_indices = finalAns[0].pickUp_indices;
    courierPath.back().end_intersection = finalAns[0].end_intersection;
    //cout<<"end intersection: " << courierPath.back().end_intersection<<endl;
    unsigned linkID = courierPath.back().end_intersection;
    courierPath.back().subpath = delData.getFoundPath(courierPath.back().start_intersection, courierPath.back().end_intersection).path;

    for(unsigned i = 1; i < finalAns.size(); i++){
        //loop through every path in the vector

        courierPath.emplace_back();
        courierPath.back().start_intersection = linkID;
        //cout<<"start intersection: " << courierPath.back().start_intersection<<endl;
        courierPath.back().pickUp_indices = finalAns[i].pickUp_indices;
        courierPath.back().end_intersection = finalAns[i].end_intersection;
        //cout<<"end intersection: " << courierPath.back().end_intersection<<endl;
        courierPath.back().subpath = delData.getFoundPath(courierPath.back().start_intersection, courierPath.back().end_intersection).path;
        linkID = courierPath.back().end_intersection;
    }
    return courierPath;
}
