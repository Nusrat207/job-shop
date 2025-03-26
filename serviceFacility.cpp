#include "serviceFacility.h"
#include "scheduler.h"
#include <iostream>

ServiceFacility::ServiceFacility(int serviceFacilityId, int serverCount, double arrivalMean, double departureMean) {
    serviceFacilityId_ = serviceFacilityId;
    serverCount_ = serverCount;
    arrivalMean_ = arrivalMean;
    departureMean_ = departureMean;
    
    // Create a shared queue for this workstation
    q = new Queue();
    
    // Create the specified number of servers
    servers.resize(serverCount);
    for (int i = 0; i < serverCount; i++) {
        servers[i] = serverInitialize(i+1);
        servers[i]->setParentFacility(this);  // Link server to this facility
        servers[i]->setQueue(q);              // Share the queue among all servers
    }
    
    // Initialize counters
    totalJobsProcessed = 0;
    busyServers = 0;
    
    // Initialize job type counters
    for (int i = 0; i < 3; i++) {
        jobTypeCount[i] = 0;
    }
    
    std::cout << "Created ServiceFacility " << serviceFacilityId_ << " with " << serverCount_ << " servers" << std::endl;
}

Server* ServiceFacility::serverInitialize(int id) {
    Server* server = new Server(id, 3); // 3 is the number of job types
    server->createTraceFile();
    server->arrivalMean() = arrivalMean_;
    server->departureMean() = departureMean_;
    server->initialize();
    return server;
}

void ServiceFacility::initializeArrival(int jobId) {
    // Record job type
    jobTypeCount[jobId-1]++;
    
    std::cout << "Facility " << serviceFacilityId_ << " received job type " << jobId << std::endl;
    
    // Find an idle server
    for (int i = 0; i < serverCount_; i++) {
        if (servers[i]->status() == 0) { // Server is idle
            std::cout << "  Assigning to idle server " << i+1 << std::endl;
            servers[i]->status() = 1; // Mark as busy immediately to prevent reassignment
            busyServers++;
            servers[i]->initializeArrival(jobId);
            return;
        }
    }
    
    // If all servers are busy, add to queue
    // The queue will be checked when a service completion occurs
    std::cout << "  All servers busy, adding to queue (length: " << q->length() << ")" << std::endl;
    Item* item = new Item();
    item->id_ = 0;  // Will be assigned by server
    item->jobId = jobId;
    item->itemArrivalTime = Scheduler::now();
    q->enqueue(item);
}

void ServiceFacility::serverCompletion(int serverId, int jobId) {
    // Mark server as free
    busyServers--;
    totalJobsProcessed++;
    
    std::cout << "Facility " << serviceFacilityId_ << " completed job type " << jobId 
              << " on server " << serverId << std::endl;
    
    // Route the job to the next workstation based on job type
    static int recursionDepth = 0;
    if (recursionDepth < 100 && nextFacility.size() >= jobId && nextFacility[jobId-1] != nullptr) {
        ServiceFacility* nextFac = nextFacility[jobId-1];
        
        // Check if the next facility is the departure server
        if (nextFac->serviceFacilityId_ == 0) {
            std::cout << "  Job exits the system" << std::endl;
            return;
        }
        
        // Check if the next facility has available servers
        bool hasAvailableServer = false;
        for (int i = 0; i < nextFac->serverCount_; i++) {
            if (nextFac->servers[i]->status() == 0) {
                hasAvailableServer = true;
                break;
            }
        }
        
        if (hasAvailableServer) {
            std::cout << "  Routing to next facility" << std::endl;
            recursionDepth++;
            nextFac->initializeArrival(jobId);
            recursionDepth--;
        } else {
            std::cout << "  Next facility is busy, job exits system to prevent deadlock" << std::endl;
        }
    } else {
        if (recursionDepth >= 100) {
            std::cout << "  WARNING: Maximum recursion depth reached, job exits system to prevent infinite loop" << std::endl;
        } else {
            std::cout << "  Job exits the system" << std::endl;
        }
    }
    
    // Check if there are waiting jobs in the queue
    if (!q->empty()) {
        Item* item = q->dequeue();
        std::cout << "  Taking next job (type " << item->jobId << ") from queue" << std::endl;
        servers[serverId-1]->initializeArrival(item->jobId);
        busyServers++;
        delete item;
    } else {
        std::cout << "  No jobs waiting in queue" << std::endl;
    }
}

void ServiceFacility::addNextFacility(ServiceFacility* s1, ServiceFacility* s2, ServiceFacility* s3) {
    nextFacility.clear();
    nextFacility.push_back(s1); // Next facility for job type 1
    nextFacility.push_back(s2); // Next facility for job type 2
    nextFacility.push_back(s3); // Next facility for job type 3
}

void ServiceFacility::setDeptServiceFac(ServiceFacility* server) {
    deptServiceFac_ = server;
}

void ServiceFacility::generateReport() {
    std::cout << "\n===============================================" << std::endl;
    std::cout << "WORKSTATION " << serviceFacilityId_ << " DETAILED REPORT" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    // Overall workstation statistics
    std::cout << "\nOVERALL STATISTICS:" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Number of servers: " << serverCount_ << std::endl;
    std::cout << "Total jobs processed: " << totalJobsProcessed << std::endl;
    std::cout << "Jobs by type: Type 1: " << jobTypeCount[0] 
              << ", Type 2: " << jobTypeCount[1] 
              << ", Type 3: " << jobTypeCount[2] << std::endl;
    std::cout << "Current queue length: " << q->size() << std::endl;
    std::cout << "Current busy servers: " << busyServers << " out of " << serverCount_ << std::endl;
    
    // Calculate workstation-level averages
    double totalUtilization = 0.0;
    double totalQueueLength = 0.0;
    double totalSystemLength = 0.0;
    double totalQueueDelay = 0.0;
    double totalSystemDelay = 0.0;
    
    // Report per server statistics
    std::cout << "\nPER-SERVER STATISTICS:" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    for (int i = 0; i < serverCount_; i++) {
        Server* server = servers[i];
        std::cout << "\nServer " << (i+1) << ":" << std::endl;
        std::cout << "  Total jobs processed: " << server->totalCustomerServed << std::endl;
        std::cout << "  Server utilization: " << (server->areaServer()/Scheduler::now()) << std::endl;
        std::cout << "  Average queue length: " << (server->areaQueue()/Scheduler::now()) << std::endl;
        std::cout << "  Average system length: " << (server->areaSystem()/Scheduler::now()) << std::endl;
        if (server->totalCustomerServed > 0) {
            std::cout << "  Average queue delay: " << (server->totalQueueDelay()/server->totalCustomerServed) << std::endl;
            std::cout << "  Average system delay: " << (server->totalSystemDelay()/server->totalCustomerServed) << std::endl;
        }
        
        // Accumulate totals for workstation averages
        totalUtilization += (server->areaServer()/Scheduler::now());
        totalQueueLength += (server->areaQueue()/Scheduler::now());
        totalSystemLength += (server->areaSystem()/Scheduler::now());
        totalQueueDelay += server->totalQueueDelay();
        totalSystemDelay += server->totalSystemDelay();
    }
    
    // Calculate and display workstation averages
    std::cout << "\nWORKSTATION AVERAGES:" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Average server utilization: " << (totalUtilization/serverCount_) << std::endl;
    std::cout << "Average queue length: " << (totalQueueLength/serverCount_) << std::endl;
    std::cout << "Average system length: " << (totalSystemLength/serverCount_) << std::endl;
    if (totalJobsProcessed > 0) {
        std::cout << "Average queue delay: " << (totalQueueDelay/totalJobsProcessed) << std::endl;
        std::cout << "Average system delay: " << (totalSystemDelay/totalJobsProcessed) << std::endl;
    }
    
    // Calculate and display traffic intensity
    double totalArrivalRate = arrivalMean_ * serverCount_;
    double totalServiceRate = departureMean_ * serverCount_;
    std::cout << "\nTRAFFIC ANALYSIS:" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Traffic intensity (ρ): " << (totalArrivalRate/totalServiceRate) << std::endl;
    
    std::cout << "\n===============================================\n" << std::endl;
}

ServiceFacility::~ServiceFacility() {
    delete q;
    for (int i = 0; i < serverCount_; i++) {
        delete servers[i];
    }
}


