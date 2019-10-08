/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MultiDestHelper.h
 * Author: nownahal
 *
 * Created on March 27, 2019, 11:26 PM
 */

#ifndef MULTIDESTHELPER_H
#define MULTIDESTHELPER_H

#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <list>
#include <boost/heap/priority_queue.hpp>
#include "StreetsDatabaseAPI.h"
#include "m1.h"
#include "Global.h"
#include "m1Helper.h"
#include "m3.h"
#include "Node.h"
#include "m4.h"
#include <chrono>

struct MDWaveElem {
    int interID;
    //time to get to node + estimated best travel time to destination
    double submitedTravelTime;
    MDWaveElem (const int& id, const double& travelCost){
        interID = id;
        submitedTravelTime = travelCost;
    }
    bool operator<( const MDWaveElem & elem ) const {
        return !(submitedTravelTime < elem.submitedTravelTime);
    }
    bool operator>( const MDWaveElem & elem ) const {
        return !(submitedTravelTime > elem.submitedTravelTime);
    }
};

struct foundPath{
    std::vector<unsigned> path;
    double timeToTravel;
    unsigned fromInterID;
    unsigned toInterID;
};

class MDNodes {
public:
    MDNodes(const unsigned intersect_id_start,double left_turn_penalty ,double right_turn_penalty);
    ~MDNodes();
    double leftTurnPenalty;
    double rightTurnPenalty;
    std::vector<bool> usedSegment;
    std::vector<double> intersBestTime;
    std::vector<int> interGetToSegment;
private:

};

//Finds the quickest time which is possible to traverse 2 intersections
double bestCaseTimePossible(int intID1, int intID2);

std::vector<foundPath> findPathMultiDest(
                    const unsigned interStartID, 
                    const vector<unsigned> endIDs,
                    const double right_turn_penalty, 
                    const double left_turn_penalty);

#endif /* MULTIDESTHELPER_H */

