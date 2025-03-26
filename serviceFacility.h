#ifndef SORUCE_CODE_SERVICEFACILITY_H
#define SORUCE_CODE_SERVICEFACILITY_H

#include "server.h"
#include "queue.h"
#include <vector>
#include <map>

class ServiceFacility {
public:
    ServiceFacility(int serviceFacilityId, int serverCount, double arrivalMean, double departureMean);
    ~ServiceFacility();
    
    // Process a job arrival at this workstation
    void initializeArrival(int jobId);
    
    // Handle server completion and route job to next workstation
    void serverCompletion(int serverId, int jobId);
    
    // Set the next facilities for each job type
    void addNextFacility(ServiceFacility* s1, ServiceFacility* s2, ServiceFacility* s3);
    
    // Set the departure service facility
    void setDeptServiceFac(ServiceFacility* server);
    
    // Generate a report of workstation statistics
    void generateReport();
    
    int serviceFacilityId_;
    int serverCount_;
    double arrivalMean_;
    double departureMean_;
    int totalJobsProcessed;
    int busyServers;
    int jobTypeCount[3];
    
    // Add method to get queue size
    int getQueueSize() const { return q->size(); }
    
private:
    Server* serverInitialize(int id);
    Queue* q;
    std::vector<Server*> servers;
    std::vector<ServiceFacility*> nextFacility;
    ServiceFacility* deptServiceFac_;
};

#endif //SORUCE_CODE_SERVICEFACILITY_H
