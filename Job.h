#ifndef JOB_H
#define JOB_H

#include <vector>

class WorkStation;

class Job {
public:
    Job(std::vector<std::pair<WorkStation*, double>>& tasks);
    std::vector<std::pair<WorkStation*, double>> getTasks() const;

private:
    std::vector<std::pair<WorkStation*, double>> tasks_;
};

#endif