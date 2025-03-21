#include "event.h"
#include "server.h"
#include "scheduler.h"
#include <iostream>

void
Event :: activate (double t) {
    stime_ = Scheduler :: now ();
    rtime_ = stime_ + t;
    Scheduler::instance().schedule(this);  // Schedule the event after setting its time
}

void
Event :: cancel () {
    status_ = 1;
}


void
ArrivalEvent :: handle () {
    server->updateStat();
    server->arrivalHandler ();
}

void
DepartureEvent :: handle () {
    server->updateStat();
    server->departureHandler ();
}

