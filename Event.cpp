#include "Event.h"
#include "Person.h"
#include "WorkStation.h"

Event::Event(int eventType, Person* person, double time, WorkStation* workstation) :
    eventType_(eventType), person_(person), time_(time), workstation_(workstation) {}

int Event::getEventType() const { return eventType_; }
Person* Event::getPerson() const { return person_; }
double Event::getTime() const { return time_; }
WorkStation* Event::getWorkStation() const { return workstation_; }
void Event::setEventType(int eventType) { eventType_ = eventType; }
void Event::setPerson(Person* person) { person_ = person; }
void Event::setTime(double time) { time_ = time; }
void Event::setWorkStation(WorkStation* workstation) { workstation_ = workstation; }