how many request can come in a given time frame and decide to allow or 
reject them based on predefined rules.

1. Code Flow
Define Creation Lambda
Receive API Request
Fetch/Create Bucket
Calculate Capacity
Allow Or Reject

2. Classes
IBucket: Rate Interface - allowRequest
LeakyBucket: Throttling Logic - allowRequest
RateLimiter: Traffic Manager - allowRequest, bucket management

3. Requirements
Functional:
Throttle Client Requests
Track Bucket State

Non-Functional:
Concurrency Thread-Safe
Open/Closed Compliant

#include <iostream>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <ctime>
#include <algorithm>
#include <functional> // Added for std::function

using namespace std;

// Defines standard contract for rate limiting algorithms.
class IBucket {
public:
    virtual bool allowRequest() = 0;
    virtual ~IBucket() = default;
};

// Implements leaky bucket throttling logic to control request bursts.
class LeakyBucket : public IBucket {
private:
    int capacity;
    double leakRate;
    double currentLevel;
    time_t lastUpdate;
    mutex mtx;

public:
    LeakyBucket(int cap, double leak) : capacity(cap), leakRate(leak), currentLevel(0), lastUpdate(time(0)) {}

    bool allowRequest() override {
        lock_guard<mutex> lock(mtx);
        time_t now = time(0);
        double leaked = (now - lastUpdate) * leakRate;
        currentLevel = max(0.0, currentLevel - leaked);
        lastUpdate = now;

        if (currentLevel + 1 <= capacity) {
            currentLevel++;
            return true;
        }
        return false;
    }
};

// Manages incoming traffic and creates new buckets dynamically.
class RateLimiter {
private:
    unordered_map<string, unique_ptr<IBucket>> partners;
    mutex globalMtx;
    
    // MODERN C++ FIX: Instead of a whole Factory Class, we just store a function!
    function<unique_ptr<IBucket>()> createBucket;

public:
    // Dependency Injection via Lambda
    RateLimiter(function<unique_ptr<IBucket>()> creatorFunc) : createBucket(creatorFunc) {}

    bool allowRequest(string name) {
        IBucket* bucket;
        
        {
            lock_guard<mutex> lock(globalMtx);
            if (partners.find(name) == partners.end()) {
                // Execute the lambda to create the specific bucket
                partners[name] = createBucket();
            }
            bucket = partners[name].get();
        }

        return bucket->allowRequest();
    }
};

// Application entry point simulating rapid API requests.
int main() {
    // 1. Define Creation Rule (Lambda)
    // We pass a simple lambda function that acts as our "Factory"
    RateLimiter rl([]() { 
        return make_unique<LeakyBucket>(10, 2.0); 
    });

    cout << "--- Simulating Rapid Requests ---\n";
    for (int i = 1; i <= 12; ++i) {
        // 2. Receive API Request
        if (rl.allowRequest("Facebook")) {
            cout << i << ". [ALLOWED]\n";
        } else {
            cout << i << ". [REJECTED]\n";
        }
    }

    return 0;
}