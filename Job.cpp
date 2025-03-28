#include "Job.h"
#include "WorkStation.h"

Job::Job(std::vector<std::pair<WorkStation*, double>>& tasks) : tasks_(tasks) {}
std::vector<std::pair<WorkStation*, double>> Job::getTasks() const { return tasks_; }