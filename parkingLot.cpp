1. Code Flow
Vehicle Arrives
Allocate Spot
Issue Ticket
Free Spot
Calculate Fee

2. Classes
Vehicle: Transport Details
Ticket: Parking Receipt
SpotStrategy: Allocation Interface
NearestFirstStrategy: Closest Spot Logic
ParkingLot: Facility Manager

3. Requirements
Functional:
Assign Available Spots
Process Vehicle Exits
Calculate Parking Fees
Non-Functional:
Algorithmically Efficient
Highly Extensible

C++
#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>

using namespace std;

enum VehicleType { TWO_WHEELER, FOUR_WHEELER };

// Represents the transport entity requiring a parking space.
class Vehicle {
public:
    string plate;
    VehicleType type;
    
    // Initializes the vehicle with its identifier and category.
    Vehicle(string p, VehicleType t) : plate(p), type(t) {}
};

// Tracks an active parking session and its associated data.
class Ticket {
public:
    string ticketId;
    int spotId;
    int entryHour;
    
    // Required for map insertion.
    Ticket() {} 
    
    // Initializes a new tracking record for a parked vehicle.
    Ticket(string t, int s, int time) : ticketId(t), spotId(s), entryHour(time) {}
};

// Defines the standard contract for parking spot allocation algorithms.
class SpotStrategy {
public:
    // Retrieves the next optimal parking spot based on the concrete implementation.
    virtual int getNextSpot(priority_queue<int, vector<int>, greater<int>>& pq) = 0;
};

// Implements allocation logic to prioritize spaces closest to the entrance.
class NearestFirstStrategy : public SpotStrategy {
public:
    // Extracts and removes the lowest available spot ID from the priority queue.
    int getNextSpot(priority_queue<int, vector<int>, greater<int>>& pq) override {
        if (pq.empty()) return -1;
        
        int nearestSpot = pq.top();
        pq.pop(); 
        return nearestSpot;
    }
};

// Manages the overall facility state, spot availability, and active tickets.
class ParkingLot {
private:
    priority_queue<int, vector<int>, greater<int>> twoWheelerSpots;
    priority_queue<int, vector<int>, greater<int>> fourWheelerSpots;
    
    unordered_map<string, Ticket> activeTickets;
    SpotStrategy* strategy;
    int ticketCounter = 1;

public:
    // Initializes the facility with specific capacities and an injected allocation strategy.
    ParkingLot(SpotStrategy* s, int twoCap, int fourCap) {
        strategy = s;
        for (int i = 1; i <= twoCap; i++) twoWheelerSpots.push(i);
        for (int i = 101; i <= 100 + fourCap; i++) fourWheelerSpots.push(i); 
    }

    // Attempts to allocate a spot and generate a ticket for an arriving vehicle.
    string park(Vehicle v, int entryHour) {
        int spot = -1;
        
        if (v.type == TWO_WHEELER) spot = strategy->getNextSpot(twoWheelerSpots);
        else spot = strategy->getNextSpot(fourWheelerSpots);

        if (spot == -1) {
            cout << "[REJECTED] Parking Full for " << v.plate << endl;
            return "";
        }

        string tId = "T" + to_string(ticketCounter++);
        activeTickets[tId] = Ticket(tId, spot, entryHour);
        
        cout << "[PARKED] " << v.plate << " got Spot " << spot << " (Ticket: " << tId << ")" << endl;
        return tId;
    }

    // Processes departing vehicles, frees their reserved spot, and calculates the final fee.
    void exit(string ticketId, int exitHour, VehicleType type) {
        if (activeTickets.find(ticketId) == activeTickets.end()) return;

        Ticket t = activeTickets[ticketId];
        activeTickets.erase(ticketId); 

        if (type == TWO_WHEELER) twoWheelerSpots.push(t.spotId);
        else fourWheelerSpots.push(t.spotId);

        int hours = exitHour - t.entryHour;
        if (hours == 0) hours = 1; 
        
        int fee = hours * (type == TWO_WHEELER ? 10 : 20);
        cout << "[EXIT] " << ticketId << " left. Fee: $" << fee << endl;
    }
};

// Application entry point demonstrating a complete parking lifecycle.
int main() {
    NearestFirstStrategy myStrategy;
    ParkingLot mallLot(&myStrategy, 2, 2);

    Vehicle car1("DL-123", FOUR_WHEELER);
    Vehicle bike1("HR-456", TWO_WHEELER);

    string t1 = mallLot.park(car1, 2);
    string t2 = mallLot.park(bike1, 2);

    mallLot.exit(t1, 5, FOUR_WHEELER);
    mallLot.exit(t2, 5, TWO_WHEELER);

    return 0;
}