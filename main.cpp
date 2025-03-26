#include <iostream>
#include <fstream>
#include <random>
#include <ctime>
using namespace std;

#include "scheduler.h"
#include "serviceFacility.h"

// Job types and their routing
// Type 1: 3, 1, 2, 5
// Type 2: 4, 1, 3
// Type 3: 2, 5, 1, 4, 3

int generateRandomJobType() {
    return rand() % 3 + 1; // Returns 1, 2, or 3
}

int main() {
    // Seed random number generator
    srand(time(nullptr));
    
    cout << "Starting Job Shop Simulation..." << endl;
    
    // Create scheduler
    Scheduler *sch = new Scheduler();
    sch->initialize();
    
    // Set simulation end time to 1000 time units
    const double MAX_SIMULATION_TIME = 1000.0;
    
    cout << "Creating service facilities..." << endl;
    
    // Create the 5 workstations with their respective server counts
    ServiceFacility* sf1 = new ServiceFacility(1, 3, 10.0, 8.0);  // Station 1 with 3 servers
    ServiceFacility* sf2 = new ServiceFacility(2, 2, 10.0, 9.0);  // Station 2 with 2 servers
    ServiceFacility* sf3 = new ServiceFacility(3, 4, 10.0, 7.0);  // Station 3 with 4 servers
    ServiceFacility* sf4 = new ServiceFacility(4, 3, 10.0, 10.0); // Station 4 with 3 servers
    ServiceFacility* sf5 = new ServiceFacility(5, 1, 10.0, 6.0);  // Station 5 with 1 server
    
    // Create a departure server to handle jobs exiting the system
    ServiceFacility* deptServer = new ServiceFacility(0, 1, 0.0, 0.0);
    
    cout << "Setting up job routing..." << endl;
    
    // Set up routing for each workstation based on job types
    // For station 1, next station for job type 1 is 2, for job type 2 is 3, for job type 3 is 4
    sf1->addNextFacility(sf2, sf3, sf4);
    sf1->setDeptServiceFac(deptServer);
    
    // For station 2, next station for job type 1 is 5, for job type 2 is nullptr (shouldn't arrive), for job type 3 is 5
    sf2->addNextFacility(sf5, nullptr, sf5);
    sf2->setDeptServiceFac(deptServer);
    
    // For station 3, next station for job type 1 is 1, for job type 2 is deptServer (exit), for job type 3 is deptServer (exit)
    sf3->addNextFacility(sf1, deptServer, deptServer);
    sf3->setDeptServiceFac(deptServer);
    
    // For station 4, next station for job type 1 is nullptr (shouldn't arrive), for job type 2 is 1, for job type 3 is 3
    sf4->addNextFacility(nullptr, sf1, sf3);
    sf4->setDeptServiceFac(deptServer);
    
    // For station 5, next station for job type 1 is deptServer (exit), for job type 2 is nullptr (shouldn't arrive), for job type 3 is 1
    sf5->addNextFacility(deptServer, nullptr, sf1);
    sf5->setDeptServiceFac(deptServer);
    
    cout << "Initializing jobs..." << endl;
    
    // Initialize first jobs for each job type according to their starting workstation
    // Job type 1 starts at station 3
    sf3->initializeArrival(1);
    
    // Job type 2 starts at station 4
    sf4->initializeArrival(2);
    
    // Job type 3 starts at station 2
    sf2->initializeArrival(3);
    
    cout << "Generating additional random jobs..." << endl;
    
    // Generate additional random jobs (reduced from 10 to 9 to prevent system overload)
    for (int i = 0; i < 9; i++) {
        int jobType = generateRandomJobType();
        
        // Assign job to the appropriate starting workstation based on job type
        if (jobType == 1) {
            sf3->initializeArrival(jobType);
        } else if (jobType == 2) {
            sf4->initializeArrival(jobType);
        } else if (jobType == 3) {
            sf2->initializeArrival(jobType);
        }
    }
    
    cout << "Running simulation for " << MAX_SIMULATION_TIME << " time units..." << endl;
    
    // Safety check to make sure we have at least one event
    if (!sch->hasEvents()) {
        cout << "Error: No events in scheduler. Cannot run simulation." << endl;
        return 1;
    }
    
    // Run the simulation with time limit
    double currentTime = 0.0;
    double lastTime = 0.0;
    int eventsProcessed = 0;
    int stuckCounter = 0;
    
    while (currentTime < MAX_SIMULATION_TIME && sch->hasEvents()) {
        lastTime = currentTime;
        currentTime = sch->runNextEvent();
        eventsProcessed++;
        
        // Check if simulation time is advancing
        if (lastTime == currentTime) {
            stuckCounter++;
            if (stuckCounter > 1000) {
                cout << "Simulation appears to be stuck, terminating." << endl;
                break;
            }
        } else {
            stuckCounter = 0;
        }
        
        // Calculate current jobs in system
        int currentJobs = sf1->busyServers + sf2->busyServers + sf3->busyServers + 
                         sf4->busyServers + sf5->busyServers;
        int queuedJobs = sf1->getQueueSize() + sf2->getQueueSize() + sf3->getQueueSize() + 
                        sf4->getQueueSize() + sf5->getQueueSize();
        
        if (eventsProcessed % 100 == 0) {
            cout << "Processed " << eventsProcessed << " events, current time: " << currentTime << endl;
            cout << "Current jobs in system: " << currentJobs << " (Processing: " << currentJobs 
                 << ", Queued: " << queuedJobs << ")" << endl;
        }
    }
    
    cout << "Simulation ended at time " << currentTime << " after processing " << eventsProcessed << " events" << endl;
    
    // Generate reports
    cout << "\n===== JOB SHOP SIMULATION REPORT =====\n";
    sf1->generateReport();
    sf2->generateReport();
    sf3->generateReport();
    sf4->generateReport();
    sf5->generateReport();
    
    // Generate summary report
    cout << "\n===============================================" << std::endl;
    cout << "SIMULATION SUMMARY REPORT" << std::endl;
    cout << "===============================================" << std::endl;
    cout << "Simulation duration: " << currentTime << " time units" << std::endl;
    cout << "Total events processed: " << eventsProcessed << std::endl;
    
    // Calculate total jobs processed across all facilities
    int totalJobs = sf1->totalJobsProcessed + sf2->totalJobsProcessed + 
                    sf3->totalJobsProcessed + sf4->totalJobsProcessed + 
                    sf5->totalJobsProcessed;
    cout << "Total jobs processed: " << totalJobs << std::endl;
    
    // Calculate average jobs per time unit
    if (currentTime > 0) {
        cout << "Average throughput: " << (totalJobs/currentTime) << " jobs per time unit" << std::endl;
    }
    
    cout << "\n===============================================\n" << std::endl;
    
    // Clean up
    delete sf1;
    delete sf2;
    delete sf3;
    delete sf4;
    delete sf5;
    delete deptServer;
    delete sch;
    
    cout << "Simulation complete." << endl;
    
    return 0;
}
