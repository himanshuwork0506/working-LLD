1. Code Flow
Initialize Theatre Venue
Start Booking Threads
Lock Seat Resource
Approve or Reject
Finish Thread Execution

2. Classes
User: Customer Details
Seat: Physical Chair
Movie: Film Details
Screen: Room Layout
Show: Thread-Safe Screening
Theatre: Venue Manager

3. Requirements
Functional:
Reserve User Seats
Prevent Double-Booking

Non-Functional:
Concurrency Thread-Safe
Data Consistent
Refactored Code

C++
#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <memory>

using namespace std;

// Represents customer details.
class User { 
public: 
    string name; 
    User(string n) : name(n) {} 
};

// Represents a physical chair in a screen.
class Seat { 
public: 
    int id;       
    Seat(int i) : id(i) {} 
};

// Represents the film being shown.
class Movie{ 
public: 
    string name; 
    Movie(string n) : name(n) {} 
};

// Groups physical seats into a single viewing room.
class Screen {
public:
    int id;
    vector<Seat> seats;

    Screen(int i) : id(i) {}
    
    // Attaches a seat to this specific screen.
    void addSeat(Seat s) { seats.push_back(s); }
};

// Manages a specific movie screening and handles thread-safe bookings.
class Show {
private:
    // Mutex lock to prevent race conditions during concurrent booking attempts.
    mutex mtx; 

public:
    int id;
    Movie movie;
    string time;
    vector<int> bookedSeats;

    Show(int i, Movie m, string t) : id(i), movie(m), time(t) {}

    // Attempts to reserve a seat, ensuring only one thread can book at a time.
    bool bookSeat(int seatId, string userName) {
        // Locks the critical section. Blocks other threads until this transaction completes.
        lock_guard<mutex> lock(mtx); 

        // Verifies if the seat is already taken.
        for (int s : bookedSeats) {
            if (s == seatId) {
                cout << "[FAILED] Sorry " << userName << ", Seat " << seatId << " is taken!\n";
                return false;
            }
        }

        // Finalizes the booking.
        bookedSeats.push_back(seatId);
        cout << "[SUCCESS] " << userName << " booked Seat " << seatId << "!\n";
        return true;
    }
};

// Top-level container managing multiple screens and shows.
class Theatre {
public:
    string name;
    vector<Screen> screens;
    vector<shared_ptr<Show>> shows; 

    Theatre(string n) : name(n) {}

    // Adds a screen room to the venue.
    void addScreen(Screen s) { screens.push_back(s); }
    
    // Adds a scheduled show to the venue.
    void addShow(shared_ptr<Show> s) { shows.push_back(s); }
};

// Helper function to execute a booking attempt asynchronously.
void bookAsync(shared_ptr<Show> show, int seatId, string user) {
    show->bookSeat(seatId, user);
}

// Application entry point simulating concurrent booking attempts.
int main() {
    // 1. Initialize Theatre Venue
    Theatre myTheatre("IMAX Downtown");

    Screen sc1(1);
    sc1.addSeat(Seat(1));
    sc1.addSeat(Seat(2));
    myTheatre.addScreen(sc1);

    auto show1 = make_shared<Show>(101, Movie("Inception"), "7:00 PM");
    myTheatre.addShow(show1);

    cout << "--- " << myTheatre.name << " is open for bookings ---\n";

    // 2. Start Booking Threads
    thread t1(bookAsync, myTheatre.shows[0], 1, "Alice");
    thread t2(bookAsync, myTheatre.shows[0], 1, "Bob");

    // 5. Finish Thread Execution
    t1.join();
    t2.join();

    return 0;
}