#ifndef EVENT_H
#define EVENT_H

class Person;
class WorkStation;

class Event {
public:
    Event(int eventType, Person* person, double time, WorkStation* workstation);
    int getEventType() const;
    Person* getPerson() const;
    double getTime() const;
    WorkStation* getWorkStation() const;
    void setEventType(int eventType);
    void setPerson(Person* person);
    void setTime(double time);
    void setWorkStation(WorkStation* workstation);

private:
    int eventType_;
    Person* person_;
    double time_;
    WorkStation* workstation_;
};

#endif