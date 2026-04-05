1. Code Flow
Initialize Factory Rules
Inject Dependency
Receive API Request
Fetch Client Bucket
Allow Or Reject

2. Classes
IBucket: Rate Interface - allowRequest
LeakyBucket: Throttling Logic   - allowRequest
IBucketFactory: Creation Interface - createBucket
LeakyBucketFactory: Concrete Creator - createBucket
RateLimiter: Traffic Manager - allowRequest, partners map

3. Requirements
Functional:
Throttle Client Requests
Track Bucket State

Non-Functional:
Concurrency Thread-Safe
Open/Closed Compliant


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
    // Mutex lock to protect bucket level state during concurrent accesses.
    mutex mtx;

public:
    LeakyBucket(int cap, double leak) : capacity(cap), leakRate(leak), currentLevel(0), lastUpdate(time(0)) {}

    bool allowRequest() override {
        // Acquires lock to safely calculate leakage and update the current volume.
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

// Defines standard contract for bucket instantiation.
class IBucketFactory {
public:
    virtual unique_ptr<IBucket> createBucket() = 0;
    virtual ~IBucketFactory() = default;
};

// Concrete creator dedicated to spinning up leaky buckets.
class LeakyBucketFactory : public IBucketFactory {
private:
    int cap;
    double leak;
public:
    LeakyBucketFactory(int c, double l) : cap(c), leak(l) {}
    
    unique_ptr<IBucket> createBucket() override {
        return make_unique<LeakyBucket>(cap, leak);
    }
};

// Manages incoming traffic and delegates evaluation to injected buckets.
class RateLimiter {
private:
    unordered_map<string, unique_ptr<IBucket>> partners;
    // Global mutex lock to prevent race conditions during new bucket creation.
    mutex globalMtx;
    IBucketFactory* factory; 

public:
    RateLimiter(IBucketFactory* f) : factory(f) {}

    bool allowRequest(string name) {
        IBucket* bucket;
        
        {
            // Locks critical section to safely read/write to the partners map.
            lock_guard<mutex> lock(globalMtx);
            if (partners.find(name) == partners.end()) {
                partners[name] = factory->createBucket();
            }
            bucket = partners[name].get();
        }

        return bucket->allowRequest();
    }
};

// Application entry point simulating rapid API requests.
int main() {
    LeakyBucketFactory myFactory(10, 2.0);
    RateLimiter rl(&myFactory);

    cout << "--- Simulating Rapid Requests ---\n";
    for (int i = 1; i <= 12; ++i) {
        if (rl.allowRequest("Facebook")) {
            cout << i << ". [ALLOWED]\n";
        } else {
            cout << i << ". [REJECTED]\n";
        }
    }

    return 0;
}