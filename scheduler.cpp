#include "scheduler.h"
#include "event.h"

#include <iostream>
#include <fstream>
using namespace std;

Event* Scheduler::eventList_;
double Scheduler::clock_;
Scheduler* Scheduler::instance_;
double Scheduler::endTime_ = 1000.0;  // Default simulation end time
bool Scheduler::isRunning_ = false;

Scheduler::Scheduler() {
    eventList_ = nullptr;
    instance_ = this;
}

double Scheduler::now() {
    return clock_;
}

Scheduler& Scheduler::instance() {
    if (!instance_) {
        instance_ = new Scheduler();
    }
    return (*instance_);
}

void Scheduler::schedule(Event* e) {
    if (!e) {
        cout << "Warning: Attempted to schedule null event" << endl;
        return;
    }

    if (e->expire() < clock_) {
        cout << "Warning: Event time " << e->expire() << " is less than current time " << clock_ << endl;
        return;
    }

    cout << "Scheduling event at time " << e->expire() << " (current time: " << clock_ << ")" << endl;
    addEvent(e);
}

void Scheduler::updateClock(double t) {
    clock_ = t;
}

void Scheduler::initialize() {
    clock_ = 0.0;
    eventList_ = nullptr;
    isRunning_ = false;
}

void Scheduler::addEvent(Event* e) {
    if (!e) return;

    // Validate event time
    if (e->expire() < clock_) {
        cout << "Warning: Event time " << e->expire() << " is less than current time " << clock_ << endl;
        return;
    }

    // Add to empty list
    if (!eventList_) {
        eventList_ = e;
        e->next_ = nullptr;
        return;
    }

    // Add as first element
    if (e->expire() < eventList_->expire()) {
        e->next_ = eventList_;
        eventList_ = e;
        return;
    }

    // Add in middle or at end
    Event* current = eventList_;
    Event* previous = nullptr;

    while (current && e->expire() >= current->expire()) {
        previous = current;
        current = current->next_;
    }

    if (previous) {
        previous->next_ = e;
    }
    e->next_ = current;
}

Event* Scheduler::removeEvent() {
    if (!eventList_) return nullptr;
    
    Event* temp = eventList_;
    eventList_ = eventList_->next_;
    return temp;
}

void Scheduler::cancel(Event* e) {
    if (!e || !eventList_) return;

    // Remove first event
    if (eventList_ == e) {
        eventList_ = eventList_->next_;
        return;
    }

    // Find and remove event
    Event* current = eventList_;
    Event* previous = nullptr;

    while (current && current != e) {
        previous = current;
        current = current->next_;
    }

    if (current) {
        previous->next_ = current->next_;
    }
}

void Scheduler::trigger() {
    Event* temp = removeEvent();
    if (temp) {
        updateClock(temp->expire());
        temp->handle();
    }
}

void Scheduler::run() {
    isRunning_ = true;
    Event* temp;

    cout << "Starting simulation..." << endl;
    cout << "Initial event list size: " << (eventList_ ? "Non-empty" : "Empty") << endl;

    while (eventList_ && isRunning_ && clock_ < endTime_) {
        temp = removeEvent();
        if (!temp) {
            cout << "No more events to process" << endl;
            break;
        }

        // Validate event time
        if (temp->expire() < clock_) {
            cout << "Warning: Event time " << temp->expire() << " is less than current time " << clock_ << endl;
            continue;
        }

        // Update clock and process event
        updateClock(temp->expire());
        cout << "Processing event at time: " << clock_ << endl;
        temp->handle();
    }

    isRunning_ = false;
    
    // Print simulation summary
    cout << "\nSimulation Summary:" << endl;
    cout << "==================" << endl;
    cout << "Simulation Time: " << clock_ << endl;
    cout << "End Time: " << endTime_ << endl;
    cout << "Events Processed: " << (eventList_ ? "Simulation stopped early" : "All events processed") << endl;
    cout << "==================" << endl;
}

void Scheduler::stop() {
    isRunning_ = false;
}

void Scheduler::setEndTime(double time) {
    endTime_ = time;
}






