Gemini said
1. Code Flow
Initialize Elevator System
Queue Floor Requests
Sweep Current Direction
Reverse When Empty
Stop at Destinations

2. Classes
ConsoleLogger: Output Formatting - logStop, logMove
IElevator: Elevator Interface - addStop, process
ScanElevator: SCAN Algorithm Logic - hasRequests, addStop, process
ElevatorController: Request Manager - requestFloor, start

3. Requirements
Functional:
Accept Floor Requests
Stop at Destinations
Sweep Directionally (SCAN)

Non-Functional:
Algorithmically Efficient
Extensible Design

C++
#include <iostream>
#include <vector>

using namespace std;

// Handles terminal output formatting.
class ConsoleLogger {
public:
    static void logStop(int floor) { cout << "[STOP] Doors open at floor " << floor << "\n"; }
    static void logMove(int floor, int dir) { 
        cout << "Moving " << (dir == 1 ? "UP" : "DOWN") << "... at " << floor << "\n"; 
    }
};

// Standard contract for elevator implementations.
class IElevator {
public:
    virtual void addStop(int floor) = 0;
    virtual void process() = 0;
    virtual ~IElevator() = default;
};

// Implements directional sweeping (SCAN) elevator logic.
class ScanElevator : public IElevator {
private:
    int currentFloor = 0, topFloor;
    int dir = 1; 
    vector<bool> stops;

    // Checks for remaining requests in a given direction.
    bool hasRequests(int checkDir) {
        for (int i = currentFloor + checkDir; i >= 0 && i <= topFloor; i += checkDir)
            if (stops[i]) return true;
        return false;
    }

public:
    // Initializes elevator limits and stop array.
    ScanElevator(int floors) : topFloor(floors), stops(floors + 1, false) {}

    // Registers a target floor request.
    void addStop(int floor) override {
        if (floor >= 0 && floor <= topFloor) stops[floor] = true;
    }

    // Executes the continuous SCAN movement loop.
    void process() override {
        while (hasRequests(1) || hasRequests(-1) || stops[currentFloor]) {
            
            // Halts at registered destinations.
            if (stops[currentFloor]) {
                ConsoleLogger::logStop(currentFloor);
                stops[currentFloor] = false;
            }

            // Reverses direction if current path is clear.
            if (dir == 1 && !hasRequests(1)) dir = -1;
            else if (dir == -1 && !hasRequests(-1)) dir = 1;

            // Advances one floor in the active direction.
            if (hasRequests(dir)) {
                currentFloor += dir;
                ConsoleLogger::logMove(currentFloor, dir);
            }
        }
    }
};

// Manages incoming requests and delegates to the active elevator.
class ElevatorController {
private:
    IElevator* elevator;
public:
    // Injects the specific elevator implementation.
    ElevatorController(IElevator* e) : elevator(e) {}
    
    // Queues a new floor request.
    void requestFloor(int floor) { elevator->addStop(floor); }
    
    // Initiates the processing loop.
    void start() { elevator->process(); }
};

// Application entry point demonstrating SCAN behavior.
int main() {
    IElevator* myElevator = new ScanElevator(10);
    ElevatorController controller(myElevator);

    controller.requestFloor(3);
    controller.requestFloor(7);
    controller.requestFloor(1);

    cout << "--- Starting SCAN Elevator ---\n";
    controller.start(); 

    delete myElevator;
    return 0;
}