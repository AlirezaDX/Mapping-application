/*
 * List to hold available drop offs and pick ups
 */

/* 
 * File:   m4Helper.h
 * Author: qinkai3
 *
 * Created on March 26, 2019, 9:52 PM
 */

#ifndef M4HELPER_H
#define M4HELPER_H

#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <list>
#include "m4.h"
#include "m3Helper.h"

#include <numeric> //for std::iota
#include <chrono>

#include <mapBin.h>

#define DELIVERY_STATE_AVAILABLE    0
#define DELIVERY_STATE_PICKED       1
#define DELIVERY_STATE_DROPPED      2

struct mySubpath{
    //Use this to create your subPath with given start intersection and the time elapsed so far 
    //and truckCapacity before getting here
    mySubpath(unsigned startIntersection, float truckCapacity, double timeElapsed){
        start_intersection = startIntersection;
        truck_capacity = truckCapacity;
        time_elapsed = timeElapsed;
    }
    mySubpath(){
    }
    // The intersection id where a start depot, pick-up intersection or drop-off intersection
    // is located
    unsigned start_intersection;

    // The intersection id where this subpath ends. This must be the
    // start_intersection of the next subpath or the intersection of an end depot
    unsigned end_intersection; //MANUAL

    // Specifies the indices from the deliveries vector of the picked up
    // delivery items at the start_intersection (if a pick up is to be made)
    // Will be length zero if no delivery item is picked up at the start intersection
    std::vector<unsigned> pickUp_indices;

    std::vector<unsigned> dropOff_indices;

    float truck_capacity;

    double time_elapsed;
};

class DeliveriesList{
    private:
        //list to hold index of which deliveries are available to be made 
        //hold id to index into deliveries vector
        std::list<int> availablePickUps;
        std::list<int> availableDropOffs;
        
        //iterator to point to first element in list that is an available pickUp
        std::list<int>::iterator availablePickUpsIt;
        //iterator to point to first element in list that is an available dropOff
        std::list<int>::iterator availableDropOffsIt;
        
        //integer to hold number of pick ups and drop offs available
        int numOfPickUps;
        int numOfDropOffs;
        
    public:
        //constructor, create a list out of all deliveries
        DeliveriesList(const std::vector<DeliveryInfo>& deliveries);
        
        //destructor, clear list of available deliveries
        ~DeliveriesList();
        
        /*<<<<<<<<<<<<<<Accessors>>>>>>>>>>>>>>*/
        
        //returns iterator to first pickup available in list
        
        std::list<int>::iterator getPickUp();
        
        //returns iterator to first drop off available in list
        std::list<int>::iterator getDropOff();
        
        //returns number of items available for pickup
        //use alongside pickup iterator to traverse through all pickups
        int getNumOfPickUps();
        
        //returns number of items available for drop off
        //use alongside drop off iterator to traverse though all drop offs
        int getNumOfDropOffs();
       
        //returns true if there are no more pickups
        bool finishedPickUps();
        
        //returns true if there are no more drop offs
        bool finishedDropOffs();
        
        /*<<<<<<<<<<<<<<Mutators>>>>>>>>>>>>>>*/
        
        //package has been picked up
        //update available pick up and available drop off list 
        // <Call the pickUp function in DeliveryRouting>
        void pickedUp(int packageID);
        
        //package has been dropped off
        //update available drop offs list
        // <Call the dropOff function in DeliveryRouting>
        void droppedOff(int packageID);
        
};

struct DR_IntersectionInfo{
    std::vector<unsigned>           pickDeliveryIDs;
    std::vector<unsigned>           dropDeliveryIDs;
    bool                            isDepot;
};

class DeliveryRouting{
      
public:
    DeliveryRouting(const std::vector<DeliveryInfo>& deliveries,
                    const std::vector<unsigned>& depots,const float right_turn_penalty,
                    const float left_turn_penalty);
    //A list that holds available deliveries and deliveries in the truck.
    DeliveriesList deliveriesList;
    const std::vector<DeliveryInfo>*    deliveriesPtr;                          
    
private:/*<<<<<<<<<<<<<<Intersection and Delivery states>>>>>>>>>>>>>>*/ 
    //Counts the intersections involved in the delivery(depot, pick up, drop off)
    unsigned                            numIntersInvolved;                      
    //Uniquely hold the intersection info structure for involved intersections
    std::vector<unsigned>               intersectionsInvolved;                  
    //Holds the indices of the 2D vector-intersectionsInvolved- Default -1 for those not involved.
    std::vector<int/*size_t*/>          intersectionsInvolvedIndex;             
    
public:
    //Number of intersections involved
    int getNumInvolvedIntersections()const;                                     
    
    //Given an intersectionID returns the index used for that intersection in
    //involvedIntersection data structures
    //Returns -1 not an involved intersection
    int getInvolvedInterIndex(int interID) const{                               
        return intersectionsInvolvedIndex[interID];
    }

private:/*<<<<<<<<<<<<<<<<<<<<<Time Computation>>>>>>>>>>>>>>>>>>>>>*/
    //A 2D vector containing the time/path between involved intersections
    std::vector<std::vector<foundPath>> computedPaths;                          
    //Sorts the indices of computedPaths for each "from" intersection in an ascending order of the paths time
    std::vector<std::vector<size_t>>    sortedComputedPathsIndex;               
    
public:
    //-----------**Use these functions for efficiency 
    double                              leftTurnPenalty;                        
    double                              rightTurnPenalty;                       
    
    //If from==to returns time 0. Path {-1}
    const foundPath& getFoundPath(int fromInterID, int toInterID)const;//---** 
    double getTime(int fromInterID, int toInterID)const;
    double getTime(int fromInterID, int toInterID, int prevSSID)const;
    double getTurnTimeOffset(int prevSSID, int nextSSID)const;//------------**
    double getTurnTimeOffset(int fromInterID, int toInterID, int prevSSID)const;
    
    //These functions return paths sorted in ascending order of their travel time
    //Index has numIntersInvolved-1 elements (0 <= index < numIntersInvolved-1)
    const foundPath& getFasterFoundPath(int fromInterID, int index) const;//------**
    double getFasterTime(int fromInterID, int index) const;
    double getFasterTime(int fromInterID, int prevSSID,
                            int index) const;
};

class DeliveryTracking{
          
public:
    //Creates a clean tracking object
    DeliveryTracking(const DeliveryRouting& delData,
                    const std::vector<unsigned>& depots);
    //Creates an object for a (partial) mySubpath input and track the path so far. (No check for legality)
    DeliveryTracking(const DeliveryRouting& delData, const std::vector<mySubpath>,
                    const std::vector<unsigned>& depots);
    //Tracks the delivery up to the input index of mySP. (No check for legality)
    DeliveryTracking(const DeliveryRouting& delData, const std::vector<mySubpath> mySP, unsigned upToIndex,
                    const std::vector<unsigned>& depots);
    
    //A list that holds available deliveries and deliveries in the truck.
    DeliveriesList deliveriesList;
    
private:/*<<<<<<<<<<<<<<Intersection and Delivery states>>>>>>>>>>>>>>*/ 
    const DeliveryRouting* delDataPtr; 
    //Holds the deliveryId of pick ups and drop offs in this intersection
    std::vector<DR_IntersectionInfo>    intersectionsInfo;
    
    //Holds the state of the Deliveries (Available,picked,dropped)
    std::vector<unsigned>               deliveryState;

public:
    //function to return info about an intersection
    const DR_IntersectionInfo& getIntersectionDeliveryInfo(int intersectionID) const;

    //Functions to update delivery states.
    void pickUp(unsigned deliveryID);
    void dropOff(unsigned deliveryID);
    
    //Updates mySP !!!!
    void pickUp(unsigned deliveryID, mySubpath& currSP);
    void dropOff(unsigned deliveryID, mySubpath& currSP);
    
    bool isAvailable(unsigned deliveryID) const;
    bool isPicked(unsigned deliveryID) const;
    bool isDropped(unsigned deliveryID) const;
    // DELIVERY_STATE_(AVAILABLE/PICKED/DROPPED)
    unsigned getDeliveryState(unsigned deliveryID) const;
    
};

int checkFasterToGoTo(unsigned j, unsigned currInt, const DeliveryRouting& delDATA, const DeliveryTracking& trackData, float truck_capacity, float truck_load );

//////////////////// <Perturbation> //////////////////
//Returns the perturbed SubPath and the time it takes to go through it.
//The time returned from this function is bugged since it does not consider the turn between subPaths
//Do not put depots in perturbation (subPath1/2 not begin/end at depot)
    // < Always: subPath1<subPath2 >
std::pair<std::vector<mySubpath>,double> createPerturbation(const std::vector<mySubpath>& baseSP, const DeliveryRouting& delData,
        unsigned subPathIdx1, unsigned subPathIdx2);

//Checks if the perturbed path is legal weight and drop wise
bool checkLegalilty(const std::vector<mySubpath>& baseCP , const std::vector<mySubpath>& newCP,
        unsigned subPath1, unsigned subPath2, const std::vector<DeliveryInfo>& deliveries,
        const float truck_capacity);

//Takes indices of sub path to perturb and takes a time offset the new times difference from the base time must be less than the offset
//If the time constraint was met and the path was legal it will apply the perturbation to the base path and return true else false
//Considers turn time from before and after the perturbation for checking the time.
bool applyTimedPerturbation(std::vector<mySubpath>& baseSP, unsigned subPath1, unsigned subPath2, double maxTimeOffset,
        const DeliveryRouting& delData,
        const float truck_capacity);

std::vector<mySubpath> perturbDeliveryOrder(std::vector<mySubpath>& path, 
                                           const std::vector<DeliveryInfo>& deliveries, 
                                           const std::vector<unsigned>& depots,
                                           const DeliveryRouting& delData,
                                           const float truck_capacity);

bool checkDeliveryOrder(std::vector<std::pair<bool, unsigned>>& path, const std::vector<DeliveryInfo>& deliveries, const float truck_capacity);

std::vector<mySubpath> greedyPath(int depotIndex, const DeliveryRouting& delData, const std::vector<DeliveryInfo>& deliveries,
const std::vector<unsigned>& depots,
const float truck_capacity);

std::vector<mySubpath> greedyPath2(std::vector<mySubpath>, const DeliveryRouting& delData, const std::vector<DeliveryInfo>& deliveries,
const std::vector<unsigned>& depots,
const float truck_capacity);


std::vector<CourierSubpath> getCourierPath(const std::vector<mySubpath>& finalAns, const DeliveryRouting& delData);

std::vector<mySubpath> enhancePath(const DeliveryRouting& delData, const MapBin& pickMap, const MapBin& dropMap, float truck_capacity);
void makeDelMap(const std::vector<DeliveryInfo>& deliveries, MapBin& pickMap, MapBin& dropMap);













#endif /* M4HELPER_H */

