//
// Created by Farhan Ishmam on 12-Nov-21.
//

#ifndef SORUCE_CODE_EVENT_H
#define SORUCE_CODE_EVENT_H

class Server;
class Scheduler;  // Forward declaration

class Event
{
public:
    Event (Server* s) : server (s), eventType_(0), stime_(0.0), rtime_(0.0), status_(0)
    {
    }

    inline int& eventType ()
    {
        return (eventType_);
    }
    inline double& expire ()
    {
        return (rtime_);
    }
    inline double& start ()
    {
        return (stime_);
    }

    void activate (double t);  // Declaration only
    void cancel ();  // Declaration only
    virtual void handle () = 0;

    Event* next_;
protected:
    Server* server;

private:
    int eventType_;	// represented by event id
    double stime_;   // start time
    double rtime_;   // run time/expiration time
    int status_;     // 0: active, 1: cancelled
};

class ArrivalEvent : public Event
{
public:
    ArrivalEvent (Server* s) : Event(s) {}
    void handle ();
};

class DepartureEvent : public Event
{
public:
    DepartureEvent (Server* s) : Event(s) {}
    void handle ();
};

#endif //SORUCE_CODE_EVENT_H
