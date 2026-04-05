1. Code Flow
Initialize Chat Instance
Spawn User Threads
Wait for Turn
Append Chat Message - store history in vector
Signal Next User

2. Classes
IChatBox: Chat Interface - addComment
TurnBasedChat: Thread-Safe Manager  - addComment, turn management, condition variable
ConsoleLogger: Output Formatter - logMessage


3. Requirements
Functional:
Sequence Chat Messages
Log User Input

Non-Functional:
Concurrency Thread-Safe
Race Condition Free

C++
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

// Handles terminal output formatting to keep I/O operations separate from core logic.
class ConsoleLogger {
public:
    static void logMessage(int id, const string& msg) {
        cout << "User " << id << " added: " << msg << "\n";
    }
};

// Defines the standard contract for any chat box implementation.
class IChatBox {
public:
    virtual void addComment(int userId, const string& msg) = 0;
    virtual ~IChatBox() = default;
};

// Manages turn-based message sequencing using strict concurrency controls.
class TurnBasedChat : public IChatBox {
private:
    vector<string> messages;
    // Mutex lock to prevent race conditions when modifying the shared message vector and turn state.
    mutex mtx;
    // Condition variable to pause execution and wake threads only when their specific turn arrives.
    condition_variable cv;
    int turn = 1; 

public:
    // Safely appends a message by blocking the thread until the correct user's turn.
    void addComment(int userId, const string& msg) override {
        // Unique lock acquires the mutex, allowing the condition variable to safely unlock/re-lock while waiting.
        unique_lock<mutex> lock(mtx);

        // Suspends the thread to avoid CPU spinning. Prevents spurious wakeups via the lambda condition.
        cv.wait(lock, [&]() { return turn == userId; });

        messages.push_back(msg);
        ConsoleLogger::logMessage(userId, msg);

        turn = (userId == 1) ? 2 : 1;
        
        // Broadcasts a wake-up signal to all suspended threads to re-evaluate their turn conditions.
        cv.notify_all(); 
    }
};

// Helper function to mimic user typing latency and submit a sequence of messages.
void simulateUser(IChatBox* chat, int id) {
    for (int i = 1; i <= 3; ++i) {
        chat->addComment(id, "Message " + to_string(i));
    }
}

// Application entry point demonstrating synchronized concurrent user chatting.
int main() {
    // 1. Initialize Chat Instance
    IChatBox* chat = new TurnBasedChat();

    // 2. Spawn User Threads
    thread t1(simulateUser, chat, 1);
    thread t2(simulateUser, chat, 2);

    t1.join();
    t2.join();

    delete chat;
    
    return 0;
}