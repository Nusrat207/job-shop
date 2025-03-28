#ifndef WORKSTATION_H
#define WORKSTATION_H

#include <queue>
#include <fstream>

class Person;
class Scheduler;

class WorkStation {
public:
    WorkStation(int workstationId, int machineCount);
    static double exponential(double mean);
    void initialize();
    void createTraceFile();
    void arrivalHandler(Person* person, Scheduler* scheduler);
    void departureHandler(Person* person, Scheduler* scheduler);
    void initializeArrival(Person* person, Scheduler* scheduler);
    void initializeDeparture(Person* person, Scheduler* scheduler);
    void updateStat();
    void report();
    
    int& status();
    double& areaQueue();
    double& areaSystem();
    double& areaWorkstation();
    double& totalQueueDelay();
    double& totalSystemDelay();

private:
    int machineCount_;
    int workstationId_;
    std::queue<Person*> queue_;
    int status_;
    std::ofstream trace_;
    double timeLastEvent;
    double areaQueue_;
    double areaWorkstation_;
    double areaSystem_;
    double queueDelay_;
    double systemDelay_;
    double totalQueueDelay_;
    double totalSystemDelay_;
    int totalJobsProcessed;
};

#endif