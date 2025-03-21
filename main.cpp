#include <iostream>
#include <fstream>
using namespace std;

#include "scheduler.h"
#include "serviceFacility.h"

int main() {
    // Initialize random seed
    srand(time(0));
    
    Scheduler* sch = new Scheduler();
    sch->initialize();
    sch->setEndTime(1000.0);  // Set simulation end time

    cout << "Creating workstations..." << endl;
    
    // Create all workstations based on the routing diagram
    ServiceFacility* sf1 = new ServiceFacility(1, 3, 10.0, 10.0);  // Workstation 1
    ServiceFacility* sf2 = new ServiceFacility(2, 2, 10.0, 10.0);  // Workstation 2
    ServiceFacility* sf3 = new ServiceFacility(3, 4, 10.0, 10.0);  // Workstation 3
    ServiceFacility* sf4 = new ServiceFacility(4, 3, 10.0, 10.0);  // Workstation 4
    ServiceFacility* sf5 = new ServiceFacility(5, 1, 10.0, 10.0);  // Workstation 5
    ServiceFacility* deptServer = new ServiceFacility(6, 1, 10.0, 10.0);  // Departure server

    // Create trace files for all facilities
    sf1->createTraceFile();
    sf2->createTraceFile();
    sf3->createTraceFile();
    sf4->createTraceFile();
    sf5->createTraceFile();
    deptServer->createTraceFile();

    cout << "Setting up routing..." << endl;
    
    // Set up routing based on the diagram
    // Job Type 1 route: 3 → 1 → 2 → 5
    // Job Type 2 route: 4 → 1 → 3
    // Job Type 3 route: 2 → 5 → 1 → 4 → 3

    // From Workstation 1
    sf1->addNextFacility(sf2, sf3, sf4);  // For job types 1,2,3 respectively
    sf1->setDeptServiceFac(deptServer);

    // From Workstation 2
    sf2->addNextFacility(sf5, nullptr, sf5);  // For job types 1,2,3 respectively
    sf2->setDeptServiceFac(deptServer);

    // From Workstation 3
    sf3->addNextFacility(sf1, deptServer, deptServer);  // For job types 1,2,3 respectively
    sf3->setDeptServiceFac(deptServer);

    // From Workstation 4
    sf4->addNextFacility(nullptr, sf1, sf3);  // For job types 1,2,3 respectively
    sf4->setDeptServiceFac(deptServer);

    // From Workstation 5
    sf5->addNextFacility(deptServer, nullptr, sf1);  // For job types 1,2,3 respectively
    sf5->setDeptServiceFac(deptServer);

    cout << "Initializing facilities..." << endl;
    
    // Initialize all facilities
    sf1->initialize();
    sf2->initialize();
    sf3->initialize();
    sf4->initialize();
    sf5->initialize();
    deptServer->initialize();

    cout << "Starting simulation with initial jobs..." << endl;
    
    // Start the simulation with initial jobs at their first workstations
    // Job Type 1 starts at Workstation 3
    sf3->serviceFacilityArrival(1);
    // Job Type 2 starts at Workstation 4
    sf4->serviceFacilityArrival(2);
    // Job Type 3 starts at Workstation 2
    sf2->serviceFacilityArrival(3);

    // Run the simulation
    sch->run();

    // Generate reports
    cout << "\nSimulation Results:\n" << endl;
    sf1->generateReport();
    sf2->generateReport();
    sf3->generateReport();
    sf4->generateReport();
    sf5->generateReport();
    deptServer->generateReport();

    // Cleanup
    delete sf1;
    delete sf2;
    delete sf3;
    delete sf4;
    delete sf5;
    delete deptServer;
    delete sch;

    return 0;
}
