#ifndef SORUCE_CODE_SERVICEFACILITY_H
#define SORUCE_CODE_SERVICEFACILITY_H

#include "server.h"
#include "queue.h"
#include <bits/stdc++.h>

class ServiceFacility {
private:
    int serviceFacilityId_;
    Queue* queue_;
    Server* s1;
    Server* s2;
    Server* s3;
    Server* s4;
    ServiceFacility* deptServiceFac_;
    vector<ServiceFacility*> nextFacility; //next facility vector for job-1,2,3
    Server* serverInitialize(int id, double arrivalMean, double departureMean);
    ofstream trace_;  // Trace file for the service facility

public:
    ServiceFacility(int serviceFacilityId, int serverCount, double arrivalMean, double departureMean);
    ~ServiceFacility();  // Add destructor to clean up servers and queue

    void serviceFacilityArrival(int jobId);
    void jobCompleted(int jobType);  // Handle routing to next facility
    void addNextFacility(ServiceFacility* s1, ServiceFacility* s2, ServiceFacility* s3);
    void setDeptServiceFac(ServiceFacility* server);
    void generateReport();
    void initialize();
    void createTraceFile();
    void updateStat();
    Queue* getQueue() { return queue_; }
    int getId() { return serviceFacilityId_; }
};

#endif //SORUCE_CODE_SERVICEFACILITY_H
