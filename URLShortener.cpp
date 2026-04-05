1. Code Flow
Initialize Encoder System
Receive Long URL
Lock Data Maps
Generate Base62 Code
Map and Return

2. Classes
IEncoder: Encoding Interface - encode
Base62Encoder: Base62 Logic - encode
TinyURL: Shortening Manager - shorten, expand, maps, mutex

3. Requirements
Functional:
Shorten Long URLs
Expand Short Codes
Prevent Duplicate Entries

Non-Functional:
Concurrency Thread-Safe
Dependency Inversion Compliant

C++
#include <iostream>
#include <unordered_map>
#include <string>
#include <mutex>

using namespace std;

// Defines the standard contract for URL encoding algorithms.
class IEncoder {
public:
    virtual string encode(int id) = 0;
    virtual ~IEncoder() = default;
};

// Implements Base62 encoding to convert integer IDs into compact alphanumeric strings.
class Base62Encoder : public IEncoder {
private:
    string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
public:
    string encode(int n) override {
        string s = "";
        while (n > 0) { s = chars[n % 62] + s; n /= 62; }
        return s.empty() ? "a" : s;
    }
};

// Manages the bi-directional mapping of URLs and safely handles concurrent access.
class TinyURL {
private:
    unordered_map<string, string> l2s, s2l; 
    int counter = 1;
    // Mutex lock to prevent race conditions when multiple threads read/write to the maps or increment the ID counter.
    mutex mtx; 
    IEncoder* encoder; 

public:
    TinyURL(IEncoder* e) : encoder(e) {}

    // Generates or retrieves a short code for a provided long URL.
    string shorten(string longUrl) {
        // Locks the critical section to ensure thread-safe map insertion and counter incrementing. Blocks concurrent calls until finished.
        lock_guard<mutex> lock(mtx); 
        
        if (l2s.count(longUrl)) return l2s[longUrl]; 

        string code = encoder->encode(counter++); 
        l2s[longUrl] = code;
        s2l[code] = longUrl;
        
        return code;
    }

    // Retrieves the original long URL associated with a given short code.
    string expand(string code) {
        // Locks the critical section to ensure thread-safe map reading.
        lock_guard<mutex> lock(mtx); 
        return s2l.count(code) ? s2l[code] : "Invalid Short URL";
    }
};

// Application entry point demonstrating URL shortening and duplicate handling.
int main() {
    // 1. Initialize Encoder System
    Base62Encoder myEncoder;
    TinyURL t(&myEncoder); 

    // 2. Receive Long URL
    string s1 = t.shorten("https://google.com");
    cout << "Shortened: " << s1 << " -> " << t.expand(s1) << endl;

    string s2 = t.shorten("https://google.com"); 
    cout << "Shortened: " << s2 << " (Notice it's the same!)" << endl;

    return 0;
}