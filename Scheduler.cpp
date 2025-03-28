#include "Scheduler.h"
#include "Event.h"
#include "WorkStation.h"
#include "Person.h"
#include <random>
#include <bits/stdc++.h>
double Scheduler::simulationTime = 0.0;

// Scheduler method implementations...
// [Rest of the Scheduler implementation from original code]

using namespace std;

Scheduler::Scheduler() {}

void Scheduler::addEvent(Event* event) {
    eventQueue_.push(event);
}

Event* Scheduler::getNextEvent() {
    if (eventQueue_.empty()) return nullptr;
    Event* nextEvent = eventQueue_.top();
    eventQueue_.pop();
    return nextEvent;
}

void Scheduler::runSimulation() {
    while (!eventQueue_.empty()) {
        Event* currentEvent = getNextEvent();
        simulationTime= currentEvent->getTime();
        WorkStation* Workstation = currentEvent->getWorkStation();
        Person* person = currentEvent->getPerson();

        if (currentEvent->getEventType() == 0) {
            Workstation->arrivalHandler(person, this);
        } else if (currentEvent->getEventType() == 1) {
            Workstation->departureHandler(person, this);
        }

        delete currentEvent;
    }
}

double Scheduler::now() { return simulationTime; }
void Scheduler::setNow(double time) { simulationTime = time; }

void Scheduler::generateArrivals(int arrivalCount, double arrivalMean, vector<Job*> jobs) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> job_dist(0, jobs.size() - 1);
    
    double currentTime = now();  // Start from current simulation time
    double lastArrivalTime = currentTime;
    
    for (int i = 0; i < arrivalCount; i++) {
        double interarrivalTime = WorkStation::exponential(arrivalMean);
        cout << arrivalMean << " " << interarrivalTime << endl;
        currentTime += interarrivalTime;
        
        Job* job = jobs[job_dist(gen)];
        
        Person* person = new Person(job, currentTime, i);
        WorkStation* nextWorkstation = person->getNext();
        if (nextWorkstation) {
            Event* arrivalEvent = new Event(0, person, currentTime, nextWorkstation);
            addEvent(arrivalEvent);
        }
        
        lastArrivalTime = currentTime;
        cout << currentTime << endl;
    }
    
}
