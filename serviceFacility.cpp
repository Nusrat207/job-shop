#include "serviceFacility.h"
#include "scheduler.h"

ServiceFacility::ServiceFacility(int serviceFacilityId, int serverCount, double arrivalMean, double departureMean) {
    serviceFacilityId_ = serviceFacilityId;
    queue_ = new Queue();
    s1 = nullptr;
    s2 = nullptr;
    s3 = nullptr;
    s4 = nullptr;
    deptServiceFac_ = nullptr;
    
    // Create servers based on facility ID
    switch(serviceFacilityId) {
        case 1:  // Workstation 1
            s1 = new Server(1, 3);
            s2 = new Server(2, 3);
            s3 = new Server(3, 3);
            s1->setQueue(queue_);
            s2->setQueue(queue_);
            s3->setQueue(queue_);
            s1->setParentFacility(this);
            s2->setParentFacility(this);
            s3->setParentFacility(this);
            s1->arrivalMean() = arrivalMean;
            s1->departureMean() = departureMean;
            s2->arrivalMean() = arrivalMean;
            s2->departureMean() = departureMean;
            s3->arrivalMean() = arrivalMean;
            s3->departureMean() = departureMean;
            break;
        case 2:  // Workstation 2
            s1 = new Server(1, 2);
            s2 = new Server(2, 2);
            s1->setQueue(queue_);
            s2->setQueue(queue_);
            s1->setParentFacility(this);
            s2->setParentFacility(this);
            s1->arrivalMean() = arrivalMean;
            s1->departureMean() = departureMean;
            s2->arrivalMean() = arrivalMean;
            s2->departureMean() = departureMean;
            break;
        case 3:  // Workstation 3
            s1 = new Server(1, 4);
            s2 = new Server(2, 4);
            s3 = new Server(3, 4);
            s4 = new Server(4, 4);
            s1->setQueue(queue_);
            s2->setQueue(queue_);
            s3->setQueue(queue_);
            s4->setQueue(queue_);
            s1->setParentFacility(this);
            s2->setParentFacility(this);
            s3->setParentFacility(this);
            s4->setParentFacility(this);
            s1->arrivalMean() = arrivalMean;
            s1->departureMean() = departureMean;
            s2->arrivalMean() = arrivalMean;
            s2->departureMean() = departureMean;
            s3->arrivalMean() = arrivalMean;
            s3->departureMean() = departureMean;
            s4->arrivalMean() = arrivalMean;
            s4->departureMean() = departureMean;
            break;
        case 4:  // Workstation 4
            s1 = new Server(1, 3);
            s2 = new Server(2, 3);
            s3 = new Server(3, 3);
            s1->setQueue(queue_);
            s2->setQueue(queue_);
            s3->setQueue(queue_);
            s1->setParentFacility(this);
            s2->setParentFacility(this);
            s3->setParentFacility(this);
            s1->arrivalMean() = arrivalMean;
            s1->departureMean() = departureMean;
            s2->arrivalMean() = arrivalMean;
            s2->departureMean() = departureMean;
            s3->arrivalMean() = arrivalMean;
            s3->departureMean() = departureMean;
            break;
        case 5:  // Workstation 5
            s1 = new Server(1, 1);
            s1->setQueue(queue_);
            s1->setParentFacility(this);
            s1->arrivalMean() = arrivalMean;
            s1->departureMean() = departureMean;
            break;
        case 6:  // Departure server
            s1 = new Server(1, 1);
            s1->setQueue(queue_);
            s1->setParentFacility(this);
            s1->arrivalMean() = arrivalMean;
            s1->departureMean() = departureMean;
            break;
    }
}

ServiceFacility::~ServiceFacility() {
    delete s1;
    delete s2;
    delete s3;
    delete s4;
    delete queue_;
}

void ServiceFacility::initialize() {
    if (s1) s1->initialize();
    if (s2) s2->initialize();
    if (s3) s3->initialize();
    if (s4) s4->initialize();
}

void ServiceFacility::createTraceFile() {
    string filename = "trace_facility_" + to_string(serviceFacilityId_) + ".txt";
    ofstream trace(filename);
    trace_ = move(trace);
}

void ServiceFacility::updateStat() {
    if (s1) s1->updateStat();
    if (s2) s2->updateStat();
    if (s3) s3->updateStat();
    if (s4) s4->updateStat();
}

void ServiceFacility::serviceFacilityArrival(int jobId) {
    cout << "ServiceFacility " << serviceFacilityId_ << " processing arrival for job type " << jobId << endl;
    
    // Initialize arrival at appropriate server
    if (s1) {
        s1->initializeArrival(jobId);
        cout << "  - Initialized arrival at Server 1" << endl;
    } else {
        cout << "  - Error: No server available to process job" << endl;
    }
    
    // Note: Do not automatically route to next facility here
    // The server will handle the routing when the job completes processing
}

Server* ServiceFacility::serverInitialize(int id, double arrivalMean, double departureMean) {
    Server* server = new Server(id, 1);  // Create a server with ID and 1 station
    server->arrivalMean() = arrivalMean;
    server->departureMean() = departureMean;
    return server;
}

void ServiceFacility::generateReport() {
    ofstream report;
    string reportName = "workstation_" + to_string(serviceFacilityId_) + ".out";
    report.open(reportName, ios::out);
    if (!report) {
        cout << "Cannot open report file for workstation " << serviceFacilityId_ << endl;
        return;
    }

    // Calculate combined statistics for the workstation
    double simulationTime = Scheduler::now();
    int totalJobs = 0;
    double totalServerUtilization = 0.0;
    double totalQueueLength = 0.0;
    double totalSystemLength = 0.0;
    double totalQueueDelay = 0.0;
    double totalSystemDelay = 0.0;
    int serverCount = 0;
    
    // Collect statistics from all servers
    if (s1) {
        totalJobs += s1->totalCustomerServed();
        totalServerUtilization += (s1->areaServer()/simulationTime)/s1->status();
        totalQueueLength += s1->areaQueue()/simulationTime;
        totalSystemLength += s1->areaSystem()/simulationTime;
        totalQueueDelay += s1->totalQueueDelay();
        totalSystemDelay += s1->totalSystemDelay();
        serverCount++;
    }
    
    if (s2) {
        totalJobs += s2->totalCustomerServed();
        totalServerUtilization += (s2->areaServer()/simulationTime)/s2->status();
        totalQueueLength += s2->areaQueue()/simulationTime;
        totalSystemLength += s2->areaSystem()/simulationTime;
        totalQueueDelay += s2->totalQueueDelay();
        totalSystemDelay += s2->totalSystemDelay();
        serverCount++;
    }
    
    if (s3) {
        totalJobs += s3->totalCustomerServed();
        totalServerUtilization += (s3->areaServer()/simulationTime)/s3->status();
        totalQueueLength += s3->areaQueue()/simulationTime;
        totalSystemLength += s3->areaSystem()/simulationTime;
        totalQueueDelay += s3->totalQueueDelay();
        totalSystemDelay += s3->totalSystemDelay();
        serverCount++;
    }
    
    if (s4) {
        totalJobs += s4->totalCustomerServed();
        totalServerUtilization += (s4->areaServer()/simulationTime)/s4->status();
        totalQueueLength += s4->areaQueue()/simulationTime;
        totalSystemLength += s4->areaSystem()/simulationTime;
        totalQueueDelay += s4->totalQueueDelay();
        totalSystemDelay += s4->totalSystemDelay();
        serverCount++;
    }
    
    // Compute averages
    double avgServerUtilization = (serverCount > 0) ? totalServerUtilization / serverCount : 0;
    double avgQueueLength = (serverCount > 0) ? totalQueueLength / serverCount : 0;
    double avgSystemLength = (serverCount > 0) ? totalSystemLength / serverCount : 0;
    double avgQueueDelay = (totalJobs > 0) ? totalQueueDelay / totalJobs : 0;
    double avgSystemDelay = (totalJobs > 0) ? totalSystemDelay / totalJobs : 0;

    // Write report
    report << "Simulation Report for Workstation " << serviceFacilityId_ << endl;
    report << "==========================================" << endl;
    report << "Simulation Time: " << simulationTime << endl;
    report << "Number of Servers: " << serverCount << endl;
    report << "Total Jobs Processed: " << totalJobs << endl;
    report << "Average Server Utilization: " << avgServerUtilization << endl;
    report << "Average Queue Length: " << avgQueueLength << endl;
    report << "Average System Length: " << avgSystemLength << endl;
    report << "Average Queue Delay: " << avgQueueDelay << endl;
    report << "Average System Delay: " << avgSystemDelay << endl;
    report << "==========================================" << endl;

    // Also print to console
    cout << "\nWorkstation " << serviceFacilityId_ << " Statistics:" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Number of Servers: " << serverCount << endl;
    cout << "Total Jobs Processed: " << totalJobs << endl;
    cout << "Average Server Utilization: " << avgServerUtilization << endl;
    cout << "Average Queue Length: " << avgQueueLength << endl;
    cout << "Average System Length: " << avgSystemLength << endl;
    cout << "Average Queue Delay: " << avgQueueDelay << endl;
    cout << "Average System Delay: " << avgSystemDelay << endl;
    cout << "----------------------------------------" << endl;

    report.close();
    
    // Also generate individual server reports
    if (s1) s1->report();
    if (s2) s2->report();
    if (s3) s3->report();
    if (s4) s4->report();
}

void ServiceFacility::addNextFacility(ServiceFacility* s1, ServiceFacility* s2, ServiceFacility* s3) {
    nextFacility.clear();  // Clear existing routes
    nextFacility.push_back(s1);
    nextFacility.push_back(s2);
    nextFacility.push_back(s3);
}

void ServiceFacility::setDeptServiceFac(ServiceFacility* server) {
    deptServiceFac_ = server;
}

void ServiceFacility::jobCompleted(int jobType) {
    cout << "Job type " << jobType << " completed at facility " << serviceFacilityId_ << endl;
    
    // Route to next facility based on job type
    if (!nextFacility.empty() && jobType <= nextFacility.size()) {
        ServiceFacility* next = nextFacility[jobType - 1];
        if (next == deptServiceFac_) {
            cout << "  - Job " << jobType << " completed, sent to departure facility" << endl;
            // Job is complete, update statistics
            updateStat();
            if (deptServiceFac_) {
                deptServiceFac_->serviceFacilityArrival(jobType);
            }
        } else if (next != nullptr) {
            cout << "  - Job " << jobType << " routed to facility " << next->getId() << endl;
            // Route to next facility
            next->serviceFacilityArrival(jobType);
        } else {
            cout << "  - Job " << jobType << " has no next facility, processing complete" << endl;
        }
    } else {
        cout << "  - Job " << jobType << " routing not defined, processing complete" << endl;
    }
}


