#include "server.h"
#include "scheduler.h"
#include "serviceFacility.h"

#include <malloc.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

Server::Server(int serverId, int stationCount) {
    queue_ = nullptr;  // Initialize to nullptr, will be set by ServiceFacility
    serverId_ = serverId;
    stationCount_ = stationCount;
    currentJobId = 0;
    arrivalMean_ = 1.0;    // Default arrival rate
    departureMean_ = 1.0;  // Default service rate
    parentFacility_ = nullptr;  // Will be set by the ServiceFacility
    
    // Create event handlers
    a_ = new ArrivalEvent(this);
    d_ = new DepartureEvent(this);
    
    // Initialize statistics
    status_ = 0;
    itemArrived_ = 0;
    timeLastEvent = 0.0;
    areaQueue_ = 0.0;
    areaServer_ = 0.0;
    areaSystem_ = 0.0;
    totalQueueDelay_ = 0.0;
    totalSystemDelay_ = 0.0;
    totalCustomerServed_ = 0;
    itemInService_ = nullptr;
}

double Server::exponential(double mean) {
    // Generate a random number between 0 and 1
    double r = (double)rand() / RAND_MAX;
    // Avoid log(0) by ensuring r is not 0
    if (r == 0.0) r = 0.000001;
    // Calculate exponential distribution: -mean * ln(1-r)
    return -mean * log(1.0 - r);
}

void Server::initialize() {
    status_ = 0;
    itemArrived_ = 0;
    timeLastEvent = 0.0;

    areaQueue_ = 0.0;
    areaServer_ = 0.0;
    areaSystem_ = 0.0;

    totalQueueDelay_ = 0.0;
    totalSystemDelay_ = 0.0;
    totalCustomerServed_ = 0;
}

void Server::initializeArrival(int jobId) {
    currentJobId = jobId;
    double t = exponential(arrivalMean_);
    double currentTime = Scheduler::now();
    trace_ << "Job " << jobId << " arrival scheduled at time " << (currentTime + t) << endl;
    cout << "Server " << serverId_ << " scheduling arrival for Job " << jobId << " at time " << (currentTime + t) << endl;
    a_->activate(t);  // This will set both start and expire times and schedule the event
}

void Server::initializeDeparture() {
    double t = exponential(departureMean_);
    double currentTime = Scheduler::now();
    trace_ << "Job " << currentJobId << " service time = " << t << endl;
    d_->activate(t);  // This will set both start and expire times
}

void Server::createTraceFile() {
    string traceName = "trace_" + to_string(serverId_) + ".out";
    trace_.open(traceName, ios::out);
    if (!trace_) {
        cout << "Cannot open trace file for server " << serverId_ << endl;
        return;
    }
    trace_ << "Trace file for Server " << serverId_ << endl;
    trace_ << "Format: <event> <time> <job_id> <server_status> <queue_size> <job_type>" << endl << endl;
}

void Server::arrivalHandler() {
    if (!queue_) {
        cout << "Error: Queue not initialized for server " << serverId_ << endl;
        return;
    }

    Item* temp = new Item();
    itemArrived_++;
    temp->id_ = itemArrived_;
    temp->itemArrivalTime = Scheduler::now();
    temp->jobType = currentJobId;

    trace_ << "ARRIVAL\t" << Scheduler::now() << "\t" << temp->id_ 
           << "\t" << status_ << "\t" << queue_->length() 
           << "\t" << temp->jobType << endl;

    if (status() < stationCount_) {
        status() += 1;
        trace_ << "SERVICE_START\t" << Scheduler::now() << "\t" << temp->id_ 
               << "\t" << status_ << "\t" << queue_->length() 
               << "\t" << temp->jobType << endl;
        
        itemInService_ = temp;
        queueDelay_ = Scheduler::now() - itemInService_->itemArrivalTime;
        totalQueueDelay_ += queueDelay_;

        initializeDeparture();
    } else {
        queue_->enque(temp);
    }
    
    updateStat();
}

void Server::departureHandler() {
    if (!itemInService_) {
        cout << "Error: No item in service for server " << serverId_ << endl;
        return;
    }

    status() -= 1;
    
    trace_ << "DEPARTURE\t" << Scheduler::now() << "\t" << itemInService_->id_ 
           << "\t" << status_ << "\t" << queue_->length() 
           << "\t" << itemInService_->jobType << endl;

    // Update statistics
    totalCustomerServed_++;
    systemDelay_ = Scheduler::now() - itemInService_->itemArrivalTime;
    totalSystemDelay_ += systemDelay_;

    // Route to next server based on job type
    int jobType = itemInService_->jobType;
    cout << "Server " << serverId_ << " completed processing job type " << jobType << endl;
    
    // If there are next servers defined within this facility, route to them
    if (!nextServer.empty() && jobType <= nextServer.size() && nextServer[jobType-1] != nullptr) {
        cout << "  - Routing to next server within facility" << endl;
        nextServer[jobType-1]->initializeArrival(jobType);
    } else {
        cout << "  - Job completed at this server" << endl;
        // Notify parent facility that job is complete at this server
        if (parentFacility_) {
            cout << "  - Notifying parent facility of job completion" << endl;
            parentFacility_->jobCompleted(jobType);
        }
    }

    // Process next item in queue if any
    if (queue_->length() > 0) {
        itemInService_ = queue_->deque();
        queueDelay_ = Scheduler::now() - itemInService_->itemArrivalTime;
        totalQueueDelay_ += queueDelay_;
        
        trace_ << "SERVICE_START\t" << Scheduler::now() << "\t" << itemInService_->id_ 
               << "\t" << status_ << "\t" << queue_->length() 
               << "\t" << itemInService_->jobType << endl;

        initializeDeparture();
    } else {
        status() = 0;
        itemInService_ = nullptr;
    }
    
    updateStat();
}

void Server::updateStat() {
    double durationSinceLastEvent = Scheduler::now() - timeLastEvent;
    timeLastEvent = Scheduler::now();

    areaQueue() += durationSinceLastEvent * (queue_->length());
    areaServer() += durationSinceLastEvent * status();
    areaSystem() += durationSinceLastEvent * (queue_->length() + status());
}

void Server::report() {
    ofstream report_;
    string reportName = "report_" + to_string(serverId_) + ".out";
    report_.open(reportName, ios::out);
    if (!report_) {
        cout << "Cannot open report file for server " << serverId_ << endl;
        return;
    }

    double simulationTime = Scheduler::now();
    double trafficIntensity = arrivalMean()/departureMean();
    double avgQueueLength = areaQueue()/simulationTime;
    double serverUtilization = (areaServer()/simulationTime)/stationCount_;
    double avgSystemLength = areaSystem()/simulationTime;
    double avgQueueDelay = totalQueueDelay()/totalCustomerServed();
    double avgSystemDelay = totalSystemDelay()/totalCustomerServed();

    report_ << "Simulation Report for Server " << serverId_ << endl;
    report_ << "==========================================" << endl;
    report_ << "Simulation Time: " << simulationTime << endl;
    report_ << "Total Jobs Processed: " << totalCustomerServed() << endl;
    report_ << "Traffic Intensity: " << trafficIntensity << endl;
    report_ << "Average Queue Length: " << avgQueueLength << endl;
    report_ << "Server Utilization: " << serverUtilization << endl;
    report_ << "Average System Length: " << avgSystemLength << endl;
    report_ << "Average Queue Delay: " << avgQueueDelay << endl;
    report_ << "Average System Delay: " << avgSystemDelay << endl;
    report_ << "==========================================" << endl;

    // Also print to console
    cout << "\nServer " << serverId_ << " Statistics:" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Total Jobs Processed: " << totalCustomerServed() << endl;
    cout << "Server Utilization: " << serverUtilization << endl;
    cout << "Average Queue Length: " << avgQueueLength << endl;
    cout << "Average System Length: " << avgSystemLength << endl;
    cout << "Average Queue Delay: " << avgQueueDelay << endl;
    cout << "Average System Delay: " << avgSystemDelay << endl;
    cout << "----------------------------------------" << endl;
}

void Server::setNext(Server* server1, Server* server2, Server* server3) {
    nextServer.clear();  // Clear existing routes
    nextServer.push_back(server1);
    nextServer.push_back(server2);
    nextServer.push_back(server3);
}

Server::~Server() {
    // Clean up event handlers
    delete a_;
    delete d_;
    
    // Clean up queue items if needed
    if (itemInService_) {
        delete itemInService_;
    }
    
    // Close trace file if open
    if (trace_.is_open()) {
        trace_.close();
    }
}
