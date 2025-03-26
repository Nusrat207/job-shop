#include "server.h"
#include "scheduler.h"
#include "serviceFacility.h"

#include <malloc.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath> // For log function
using namespace std;

Server::Server(int serverId, int stationCount) : a_(this), d_(this)
{
    queue_ = nullptr; // Will be set by the ServiceFacility
    serverId_ = serverId;
    stationCount_ = stationCount;
    parentFacility = nullptr;
}

double
Server::exponential(double mean) {
    double r = (double)rand()/(RAND_MAX + 1.0);
    double ex = -log(r)/mean;
    return ex;
}

void
Server::initialize() {
    status_ = 0;
    itemArrived_ = 0;
    timeLastEvent = 0.0;

    areaQueue_ = 0.0;
    areaServer_ = 0.0;
    areaSystem_ = 0.0;

    totalQueueDelay_ = 0.0;
    totalSystemDelay_ = 0.0;
    totalCustomerServed = 0;
}

void Server::setParentFacility(ServiceFacility* facility) {
    parentFacility = facility;
}

void Server::setQueue(Queue* queue) {
    queue_ = queue;
}

void Server::initializeArrival(int jobId) {
    currentJobId = jobId;
    updateStat();
    double t = exponential(arrivalMean_);
    trace_ << "interarrival time " << t << endl;
    a_.activate(t);
}

void Server::initializeDeparture() {
    updateStat();
    double t = exponential(departureMean_);
    trace_ << "\tservice time = " << t << endl;
    d_.activate(t);
}

void
Server::createTraceFile() {
    string traceName = "trace_" + to_string(serverId_) + ".out";
    trace_.open(traceName, ios::out);
    if (!trace_) {
        cout << "cannot open the trace file.\n";
    }
    trace_ << "trace file for the simulation" << endl;
    trace_ << "format of the file" << endl;
    trace_ << "<event> <time> <item id> <server status> <queue size>" << endl << endl;
}

void
Server::arrivalHandler() {
    Item* temp;
    itemArrived_++;
    temp = new Item();
    temp->id_ = itemArrived_;
    temp->jobId = currentJobId;
    temp->itemArrivalTime = Scheduler::now();

    trace_ << "a\t" << Scheduler::now() << "\t" << temp->id_ << "\t" << status_ << "\t" << queue_->length() << endl;

    // Status should already be set to 1 (busy) by ServiceFacility before this is called
    // but we'll check to be safe
    if (status() == 0) {
        status() = 1;
    }
    
    trace_ << "s\t" << Scheduler::now() << "\t" << temp->id_ << "\t" << status_ << "\t" << queue_->length() << endl;
    itemInService_ = temp;
    queueDelay_ = Scheduler::now() - itemInService_->itemArrivalTime;
    totalQueueDelay_ += queueDelay_;

    initializeDeparture();
}

void
Server::departureHandler() {
    status() = 0;
    int completedJobId = itemInService_->jobId;
    
    // Write to the trace file
    trace_ << "d\t" << Scheduler::now() << "\t" << itemInService_->id_ << "\t" << status_ << "\t" << queue_->length() << endl;
    
    // Update output statistics
    totalCustomerServed++;
    systemDelay_ = Scheduler::now() - itemInService_->itemArrivalTime;
    totalSystemDelay_ += systemDelay_;
    
    // If this server belongs to a ServiceFacility, notify it about completion
    if (parentFacility != nullptr) {
        parentFacility->serverCompletion(serverId_, completedJobId);
    }
    
    // Check if there are waiting jobs in the queue
    if (queue_->length() > 0) {
        itemInService_ = queue_->dequeue();
        currentJobId = itemInService_->jobId;
        
        // Update output statistics
        queueDelay_ = Scheduler::now() - itemInService_->itemArrivalTime;
        totalQueueDelay_ += queueDelay_;
        
        // Write to the trace file
        trace_ << "s\t" << Scheduler::now() << "\t" << itemInService_->id_ << "\t" << 1 << "\t" << queue_->length() << endl;
        status() = 1;
        
        initializeDeparture();
    }
    else {
        status() = 0;
        itemInService_ = nullptr;
    }
}

void Server::updateStat()
{
    double durationSinceLastEvent;

    durationSinceLastEvent = Scheduler::now() - timeLastEvent;
    timeLastEvent = Scheduler::now();

    areaQueue() += durationSinceLastEvent*(queue_ ? queue_->length() : 0);
    areaServer() += durationSinceLastEvent*status();
    areaSystem() += durationSinceLastEvent*((queue_ ? queue_->length() : 0) + status());
}

void Server::report()
{
    ofstream report_;
    string reportName = "report_" + to_string(serverId_) + ".out";
    report_.open(reportName, ios::out);
    if (!report_) {
        cout << "cannot open the report file.\n";
    }
    report_ << "Report for Server " << serverId_ << endl;
    report_ << "Traffic Intensity: " << arrivalMean()/departureMean() << endl;
    report_ << "Average Number of Customers in the Queue: " << (areaQueue()/(Scheduler::now())) << endl;
    report_ << "Average Server Utilization: " << (areaServer()/(Scheduler::now())) << endl;
    report_ << "Average Number of Customers in the System: " << (areaSystem()/(Scheduler::now())) << endl;
    report_ << "Average Queueing Delay: " << (totalQueueDelay()/totalCustomerServed) << endl;
    report_ << "Average System Delay: " << (totalSystemDelay()/totalCustomerServed) << endl;
    report_ << "Total Customers Served: " << totalCustomerServed << endl;
    
    // Also output to console for easier viewing
    cout << "  Server " << serverId_ << " - Total Jobs: " << totalCustomerServed 
         << " Utilization: " << (areaServer()/(Scheduler::now())) << endl;
}

Server::~Server() {
    if (trace_.is_open()) {
        trace_.close();
    }
}
