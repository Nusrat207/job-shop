#include "Scheduler.h"
#include "WorkStation.h"
#include "Job.h"
#include <vector>
#include <ctime>

int main() {
    srand(time(0));
    
    // Create workstations
    WorkStation* workstation1 = new WorkStation(1, 3);
    WorkStation* workstation2 = new WorkStation(2, 2);
    WorkStation* workstation3 = new WorkStation(3, 4);
    WorkStation* workstation4 = new WorkStation(4, 3);
    WorkStation* workstation5 = new WorkStation(5, 1);

    // Create trace files
    workstation1->createTraceFile();
    workstation2->createTraceFile();
    workstation3->createTraceFile();
    workstation4->createTraceFile();
    workstation5->createTraceFile();

    // Create jobs
    std::vector<Job*> jobs(3);
    std::vector<std::pair<WorkStation*, double>> tasks1 = 
        {{workstation3, 0.5}, {workstation1, 0.6}, {workstation2, 0.85}, {workstation5, 0.5}};
    std::vector<std::pair<WorkStation*, double>> tasks2 = 
        {{workstation4, 1.1}, {workstation1, 0.8}, {workstation3, 0.75}};
    std::vector<std::pair<WorkStation*, double>> tasks3 = 
        {{workstation2, 1.2}, {workstation5, 0.25}, {workstation1, 0.7}, {workstation4, 0.9}, {workstation3, 1.0}};

    jobs[0] = new Job(tasks1);
    jobs[1] = new Job(tasks2);
    jobs[2] = new Job(tasks3);

    // Run simulation
    Scheduler* scheduler = new Scheduler();
    scheduler->generateArrivals(1000, 0.7, jobs);
    scheduler->runSimulation();

    // Generate reports
    workstation1->report();
    workstation2->report();
    workstation3->report();
    workstation4->report();
    workstation5->report();

    // Clean up
    for (auto job : jobs) delete job;
    delete workstation1; delete workstation2; delete workstation3;
    delete workstation4; delete workstation5;
    delete scheduler;

    return 0;
}

// g++ -o out main.cpp Event.cpp Scheduler.cpp Job.cpp Person.cpp WorkStation.cpp