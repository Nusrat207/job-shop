#include "WorkStation.h"
#include "Person.h"
#include "Scheduler.h"
#include "Job.h"
#include <bits/stdc++.h>
#include <iostream>
#include <cmath>
#include <random>
#include <fstream>
#include <queue>
#include <ctime>

#include<string.h>
#include <string>

using namespace std;

WorkStation::WorkStation(int workstationId, int machineCount) : 
    workstationId_(workstationId), machineCount_(machineCount) {
    status_ = 0;
    timeLastEvent = 0.0;
    areaQueue_ = 0.0;
    areaWorkstation_ = 0.0;
    areaSystem_ = 0.0;
    totalQueueDelay_ = 0.0;
    totalSystemDelay_ = 0.0;
    totalJobsProcessed= 0;
}

double WorkStation::exponential(double mean) {
    double r;
    do {
        r = (double)rand() / (RAND_MAX + 1.0);
    } while (r == 0.0); // Avoid log(0)

    return -log(r) / mean;
}

void WorkStation::initialize() {
    status_ = 0;
    timeLastEvent = 0.0;
    areaQueue_ = 0.0;
    areaWorkstation_ = 0.0;
    areaSystem_ = 0.0;
    totalQueueDelay_ = 0.0;
    totalSystemDelay_ = 0.0;
    totalJobsProcessed = 0;
}

void WorkStation::createTraceFile() {
    string traceName = "trace_" + to_string(workstationId_ ) + ".out";
    trace_.open(traceName, ios::out);
    if (!trace_) {
        cout << "Cannot open trace file for server " << workstationId_ << endl;
    }
    trace_ << "Trace file for server " << workstationId_  << endl;
    trace_ << "Format: <event> <time> <item id> <server status> <queue size>" << endl << endl;
}

void WorkStation::arrivalHandler(Person* person, Scheduler* scheduler) {
    trace_ << "a\t" << person->getEventTime() << "\t" << person->getID() 
           << "\t" << status_ << "\t" << queue_.size() << endl;
    updateStat();
    
    if (status() < machineCount_) {
        status() += 1;
        trace_ << "s\t" << Scheduler::now() << "\t" << person->getID() 
               << "\t" << status_ << "\t" << queue_.size() << endl;
        queueDelay_ = Scheduler::now() - person->getLastArrivalTime();
        totalQueueDelay_ += queueDelay_;
        initializeDeparture(person, scheduler);
    } else {
        queue_.push(person);
    }
}

void WorkStation::departureHandler(Person* person, Scheduler* scheduler) {
    updateStat();
    // status() -= 1;
    trace_ << "d\t" << Scheduler::now() << "\t" << person->getID() 
           << "\t" << status_ << "\t" << queue_.size() << endl;
    
    totalJobsProcessed++;
    systemDelay_ = Scheduler::now() - person->getLastArrivalTime();
    totalSystemDelay_ += systemDelay_;

    if (!queue_.empty()) {
        Person* nextPerson = queue_.front();
        queue_.pop();
        queueDelay_ = Scheduler::now() - nextPerson->getLastArrivalTime();
        totalQueueDelay_ += queueDelay_;
        trace_ << "s\t" << Scheduler::now() << "\t" << nextPerson->getID() 
               << "\t" << status_ << "\t" << queue_.size() << endl;
        initializeDeparture(nextPerson, scheduler);
    }
    else{
        status() -= 1;
    }
    
    person->setJobDone(person->getJobDone() + 1);
    if (person->getJobDone() < person->getJob()->getTasks().size()) {
        initializeArrival(person, scheduler);
    }
}

void WorkStation::initializeArrival(Person* person, Scheduler* scheduler) {
    if (person->getJobDone() < person->getJob()->getTasks().size()) {
        WorkStation* nextworkstation = person->getNext();
        if (nextworkstation) {
            Event* arrivalEvent = new Event(0, person, Scheduler::now(), nextworkstation);
            person->setLastArrivalTime(Scheduler::now());
            scheduler->addEvent(arrivalEvent);
        }
    }
}

void WorkStation::initializeDeparture(Person* person, Scheduler* scheduler) {
    if (person->getJobDone() < person->getJob()->getTasks().size()) {
        double serviceTime = exponential(person->getJob()->getTasks()[person->getJobDone()].second);
        trace_ << "\tService time = " << serviceTime << endl;
        Event* departureEvent = new Event(1, person, Scheduler::now() + serviceTime, this);
        scheduler->addEvent(departureEvent);
    }
}

void WorkStation::updateStat() {
    double timeSinceLastEvent = Scheduler::now() - timeLastEvent;
    if(timeSinceLastEvent<0)cout << timeSinceLastEvent << endl;
    timeLastEvent = Scheduler::now();
    
    areaQueue() += timeSinceLastEvent * queue_.size();
    areaWorkstation() += timeSinceLastEvent * status();
    areaSystem() += timeSinceLastEvent * (queue_.size() + status());
}

void WorkStation::report() {
    ofstream report_;
    string reportName = "report_" + to_string(workstationId_) + ".out";
    report_.open(reportName, ios::out);
    if (!report_) {
        cout << "Cannot open report file for server " << workstationId_ << endl;
        return;
    }
    
    report_ << "Simulation Report for Workstation " << workstationId_ << endl;
    report_ << "---------------------------------" << endl;
    report_ << "total queue length: " << (areaQueue() ) << endl;
    report_ << "total system customers: " << (areaSystem() ) << endl;
    report_ << "total server utilization: " << (areaWorkstation() ) / machineCount_ << endl;
    report_ << "Average queue length: " << (areaQueue() / Scheduler::now()) << endl;
    report_ << "Server utilization: " << (areaWorkstation() / Scheduler::now()) / machineCount_ << endl;
    report_ << "Average system customers: " << (areaSystem() / Scheduler::now()) << endl;
    if (totalJobsProcessed > 0) {
        report_ << "Average queue delay: " << (totalQueueDelay() / totalJobsProcessed) << endl;
        report_ << "Average system delay: " << (totalSystemDelay() / totalJobsProcessed) << endl;
    }
    report_ << "Total customers served: " << totalJobsProcessed<< endl;
}

int& WorkStation::status() { return status_; }
double& WorkStation::areaQueue() { return areaQueue_; }
double& WorkStation::areaSystem() { return areaSystem_; }
double& WorkStation::areaWorkstation() { return areaWorkstation_; }
double& WorkStation::totalQueueDelay() { return totalQueueDelay_; }
double& WorkStation::totalSystemDelay() { return totalSystemDelay_; }
