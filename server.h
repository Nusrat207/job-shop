//
// Created by Farhan Ishmam on 12-Nov-21.
//

#ifndef SORUCE_CODE_SERVER_H
#define SORUCE_CODE_SERVER_H

#include "event.h"
#include "queue.h"
#include <bits/stdc++.h>
//#include "scheduler.h"

#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

// Forward declaration
class ServiceFacility;
class ArrivalEvent;
class DepartureEvent;

class Server
{
public:

    Server (int serverId, int stationCount);
    ~Server();  // Destructor to clean up memory
    void initialize ();

    void initializeArrival(int jobId);
    void initializeDeparture();

    void arrivalHandler ();
    void departureHandler ();
    void createTraceFile ();

    void updateStat();
    void report();

    void setNext(Server *server1, Server* server2, Server* server3);
    void setQueue(Queue* queue) { queue_ = queue; }
    void setParentFacility(ServiceFacility* facility) { parentFacility_ = facility; }

    inline int& status (){
        return (status_);
    }
    inline int& itemArrived (){
        return (itemArrived_);
    }
    inline double& arrivalMean (){
        return (arrivalMean_);
    }
    inline double& departureMean (){
        return (departureMean_);
    }

    //Time Average Functions
    inline double& areaQueue(){
        return (areaQueue_);
    }
    inline double& areaSystem(){
        return (areaSystem_);
    }
    inline double& areaServer(){
        return (areaServer_);
    }
    //Job Average Functions
    inline double& totalQueueDelay(){
        return (totalQueueDelay_);
    }
    inline double& totalSystemDelay(){
        return (totalSystemDelay_);
    }
    inline int& totalCustomerServed() { return totalCustomerServed_; }

protected:
    Queue* queue_;
    int serverId_;
    int stationCount_;
    int status_; //number of busy servers
    int itemArrived_; // count of already arrived events
    Item* itemInService_; // holds reference to the current item (event) in service
    ofstream trace_; // used for writing to the trace file

    double arrivalMean_; // mean time for arrival events
    double departureMean_; // mean time for departure events

    //Variables for Time Average
    double timeLastEvent;
    double areaQueue_;
    double areaServer_;
    double areaSystem_;


    //Variables for Job Average
    double queueDelay_;
    double systemDelay_;
    double totalQueueDelay_;
    double totalSystemDelay_;
    int totalCustomerServed_;

    vector<Server*> nextServer;
    int currentJobId;
    ServiceFacility* parentFacility_;  // Reference to parent facility for routing
    ArrivalEvent* a_;     // Arrival event
    DepartureEvent* d_;   // Departure event

private:
    double exponential(double mean);
};


#endif //SORUCE_CODE_SERVER_H
