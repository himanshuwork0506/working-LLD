how many request can come in a given time frame and decide to allow or 
reject them based on predefined rules.

1. Code Flow (Direct Approach)
Initialize Rate Limiter with Capacity & Leak rules
Receive API Request
Fetch/Create Bucket directly inside the manager
Calculate Capacity
Allow Or Reject

2. Classes
IBucket: Rate Interface
LeakyBucket: Throttling Logic
RateLimiter: Traffic Manager (Now directly builds LeakyBuckets)

3. Requirements
Functional:

Throttle Client Requests
Track Bucket State

Non-Functional:
Concurrency Thread-Safe
~~Open/Closed Compliant~~ (Trade-off: We are sacrificing this principle for simplicity. The manager is now tightly coupled to the LeakyBucket).


C++
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <ctime>
#include <algorithm>

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
    
    // Instead of a function, we just store the configuration values
    int defaultCapacity;
    double defaultLeakRate;

public:
    // Pass the raw configuration numbers directly into the constructor
    RateLimiter(int cap, double leak) : defaultCapacity(cap), defaultLeakRate(leak) {}

    bool allowRequest(string name) {
        IBucket* bucket;
        
        {
            lock_guard<mutex> lock(globalMtx);
            if (partners.find(name) == partners.end()) {
                // The manager directly creates the LeakyBucket itself
                partners[name] = make_unique<LeakyBucket>(defaultCapacity, defaultLeakRate);
            }
            bucket = partners[name].get();
        }

        return bucket->allowRequest();
    }
};

// Application entry point simulating rapid API requests.
int main() {
    // 1. Initialize with raw numbers instead of a lambda
    RateLimiter rl(10, 2.0);

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