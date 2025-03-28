#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#include <vector>
#include "Event.h"  

class Event;
class Person;
class Job;

class Scheduler {
public:
    static double simulationTime;
    
    Scheduler();
    void addEvent(Event* event);
    Event* getNextEvent();
    void runSimulation();
    static double now();
    static void setNow(double time);
    void generateArrivals(int arrivalCount, double arrivalMean, std::vector<Job*> jobs);

private:
    struct CompareEvent {
        bool operator()(Event* e1, Event* e2) {
            return e1->getTime() > e2->getTime();
        }
    };
    std::priority_queue<Event*, std::vector<Event*>, CompareEvent> eventQueue_;
};

#endif