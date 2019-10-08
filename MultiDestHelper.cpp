/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MultiDestNodes.cpp
 * Author: nownahal
 * 
 * Created on March 27, 2019, 11:26 PM
 */

#include <vector>

#include "MultiDestHelper.h"

MDNodes::MDNodes(const unsigned intersect_id_start,double left_turn_penalty ,double right_turn_penalty) {
    int maxInt = 0;
    maxInt = ~maxInt;
    maxInt = (unsigned int)maxInt >> 1;
    //std::cout << "Max integer:" << maxInt << std::endl;
    intersBestTime.resize(getNumIntersections(),maxInt);
    intersBestTime[intersect_id_start] = 0;
    usedSegment.resize(getNumStreetSegments(), false);
    interGetToSegment.resize(getNumIntersections(),-2);
    interGetToSegment[intersect_id_start] = -1;
    leftTurnPenalty = left_turn_penalty;
    rightTurnPenalty = right_turn_penalty;
}

MDNodes::~MDNodes() {
}


//Finds the quickest time which is possible to traverse 2 intersections
double bestCaseTimePossible(int intID1, int intID2){
    LatLon pos1 = getIntersectionPosition(intID1);
    LatLon pos2 = getIntersectionPosition(intID2);
    return find_distance_between_two_points(pos1, pos2)/(double)_DATA.maxSpeedLimit;
}


std::vector<foundPath> findPathMultiDest(
                    const unsigned interStartID, 
                    const vector<unsigned> endIDs,
                    const double right_turn_penalty, 
                    const double left_turn_penalty){

    std::vector<foundPath> ans;
    if(interStartID>=(unsigned)getNumIntersections())
    {
        std::cout << "Invalid inputs to findPathMultiDest."<< std::endl;
        return ans; //empty
    }
    
    ans.resize(endIDs.size());
    
    MDNodes myNodes(interStartID, left_turn_penalty, right_turn_penalty);
    
    boost::heap::priority_queue<MDWaveElem> waveFront;  //priority queue inclaudes nodes to check
    //boost::heap::priority_queue<WaveElem,std::vector<WaveElem>, std::greater<WveElem>> waveFront;

    waveFront.push(MDWaveElem(interStartID,0)); //First element to be popped
    //double minDestTime = 0;
    
    while(!waveFront.empty()){
        int currInter = waveFront.top().interID;
        double currInterBestTime = myNodes.intersBestTime[currInter];
        int currInterPrevSeg = myNodes.interGetToSegment[currInter];
        waveFront.pop();
        int currNodeNumAdjSS = _DATA.intersectionStreetSegment[currInter].size();
		//Go through adjacent intersections to currNode and create/update their nodes and push them in the WaveFront if eligible
        for(int i=0; i<currNodeNumAdjSS; i++){
            const int& adjSSID = _DATA.intersectionStreetSegment[currInter][i];
            if(myNodes.usedSegment[adjSSID])
                continue;
            const InfoStreetSegment& ssInfo = _DATA.streetSegmentInfo[adjSSID];
            int adjInterID;
            if(ssInfo.from == currInter){
                adjInterID = ssInfo.to;
            }
            else if(!ssInfo.oneWay)
                adjInterID = ssInfo.from;
            else continue;

            myNodes.usedSegment[adjSSID] = true;

            if(adjInterID == currInter) {continue;}

            double ssTurnTravelTime = _DATA.streetSegmentTravelTime[adjSSID];
            
            if( currInterPrevSeg != -1 && _DATA.streetSegmentInfo[currInterPrevSeg].streetID != ssInfo.streetID) //Applicable to M4 only else VV
                ssTurnTravelTime += right_turn_penalty;
            
            /*if(currInterPrevSeg != -1 && find_turn_type(currInterPrevSeg, adjSSID) != TurnType::STRAIGHT){ //If it is not the first node
                if(find_turn_type(currInterPrevSeg, adjSSID) == TurnType::RIGHT) //redundant call the function once.
                    ssTurnTravelTime += right_turn_penalty;
                else if(find_turn_type(currInterPrevSeg, adjSSID) == TurnType::LEFT)
                    ssTurnTravelTime += left_turn_penalty;
            }*/
            double timeToNode = ssTurnTravelTime + currInterBestTime;

            if(timeToNode< myNodes.intersBestTime[adjInterID]){ //Will the new time be an improvement?
                myNodes.intersBestTime[adjInterID] = timeToNode;
                myNodes.interGetToSegment[adjInterID] = adjSSID;
                waveFront.push(MDWaveElem(adjInterID,timeToNode));
            }
        } 
    }
    for(unsigned i=0; i<endIDs.size(); i++){
        ans[i].timeToTravel = myNodes.intersBestTime[endIDs[i]];
        ans[i].fromInterID = interStartID;
        ans[i].toInterID = endIDs[i];
    }
    

    for(unsigned i=0; i<endIDs.size(); i++){
        unsigned lastInter = endIDs[i];
        unsigned prevInter; //get the index of the preceding node
        while(lastInter != interStartID){
            int prevEdge = myNodes.interGetToSegment[lastInter]; //get the index of the preceding edge
            if(prevEdge == -2){ //If no path was found.
                break;
            }
                
            ans[i].path.push_back(prevEdge); //store the segment ID
            const InfoStreetSegment& prevSSInfo = _DATA.streetSegmentInfo[prevEdge];
            //Find previous intersection
            if((unsigned)prevSSInfo.from == lastInter) 
               prevInter = (unsigned)prevSSInfo.to; 
            else 
               prevInter = (unsigned)prevSSInfo.from;

            lastInter = prevInter;
        }
        std::reverse(ans[i].path.begin(), ans[i].path.end());
    }
    
    return ans;
}
