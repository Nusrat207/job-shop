//
// Created by Farhan Ishmam on 12-Nov-21.
//

#ifndef SORUCE_CODE_SERVER_H
#define SORUCE_CODE_SERVER_H

#include "event.h"
#include "queue.h"
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

// Forward declaration to avoid circular dependency
class ServiceFacility;

class Server
{
public:
    Server(int serverId, int stationCount);
    ~Server();
    
    void initialize();
    void createTraceFile();
    void initializeArrival(int jobId);
    void initializeDeparture();
    void arrivalHandler();
    void departureHandler();
    void updateStat();
    void report();
    
    void setParentFacility(ServiceFacility* facility);
    void setQueue(Queue* queue);
    
    inline int& status() { return status_; }
    inline double& arrivalMean() { return arrivalMean_; }
    inline double& departureMean() { return departureMean_; }
    inline double& areaQueue() { return areaQueue_; }
    inline double& areaServer() { return areaServer_; }
    inline double& areaSystem() { return areaSystem_; }
    inline double& totalQueueDelay() { return totalQueueDelay_; }
    inline double& totalSystemDelay() { return totalSystemDelay_; }
    
    int totalCustomerServed;  // Made public
    
private:
    double exponential(double mean);
    Queue* queue_;
    int serverId_;
    int stationCount_;
    ServiceFacility* parentFacility;
    
    int status_;
    int itemArrived_;
    double timeLastEvent;
    
    double arrivalMean_;
    double departureMean_;
    
    double areaQueue_;
    double areaServer_;
    double areaSystem_;
    
    double totalQueueDelay_;
    double totalSystemDelay_;
    
    double queueDelay_;
    double systemDelay_;
    
    Item* itemInService_;
    int currentJobId;
    
    std::ofstream trace_;
    
    ArrivalEvent a_;
    DepartureEvent d_;
};

#endif //SORUCE_CODE_SERVER_H
