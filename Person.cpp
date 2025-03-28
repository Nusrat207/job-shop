#include "Person.h"
#include "Job.h"
#include "WorkStation.h"

Person::Person(Job* job, double lastArrivalTime, int id) : 
    job_(job), jobDone_(0), eventTime_(lastArrivalTime), id_(id), lastArrivalTime_(lastArrivalTime) {}

Job* Person::getJob() const { return job_; }
void Person::setJob(Job* job) { job_ = job; }
int Person::getJobDone() const { return jobDone_; }
void Person::setJobDone(int count) { jobDone_ = count; }
double Person::getEventTime() const { return eventTime_; }
void Person::setEventTime(double time) { eventTime_ = time; }
int Person::getID() const { return id_; }
double Person::getLastArrivalTime() const { return lastArrivalTime_; }
void Person::setLastArrivalTime(double time) { lastArrivalTime_ = time; }

WorkStation* Person::getNext() {
    auto tasks = job_->getTasks();
    if (jobDone_ < tasks.size()) return tasks[jobDone_].first;
    return nullptr;
}

WorkStation* Person::getCurrent() {
    auto tasks = job_->getTasks();
    if (jobDone_ > 0) return tasks[jobDone_ - 1].first;
    return tasks[0].first;
}