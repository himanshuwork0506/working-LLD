1. Code Flow
Create Nodes: Initialize 2000, 500, and 100 dispensers.
Link Chain: Connect them in descending order.
Start Request: Pass the ₹2600 request to the 2000 node.
Process & Pass: Each node dispenses its notes and passes the remainder down.
Resolve: Print the receipt if successful, or throw an error if money is leftover.

2. Classes
IDispenser: Standard interface for all dispensers. setNext,dispense
CashDispenser: Handles math and chain delegation.
ATMLogger: Handles console output.

3. Requirements
Functional:
Calculate and dispense notes.
Throw error for invalid amounts.
Print successful receipt.

Non-Functional:
Extensible (easy to add new notes).
Reusable (one class handles all notes).
Refactored Code

C++
#include <iostream>
#include <memory>
#include <map>
#include <stdexcept>

using namespace std;

// Defines standard contract for any dispenser.
class IDispenser {
public:
    // Links current dispenser to the next.
    virtual void setNext(shared_ptr<IDispenser> next) = 0;
    
    // Processes amount and populates results.
    virtual void dispense(int amount, map<int, int>& result) = 0;
    
    // Ensures memory cleanup.
    virtual ~IDispenser() = default;
};

// Concrete implementation handling dynamic dispensing.
class CashDispenser : public IDispenser {
private:
    int denomination;
    shared_ptr<IDispenser> nextDispenser;

public:
    // Initializes with specific currency value.
    CashDispenser(int val) : denomination(val) {}

    // Sets next handler in chain.
    void setNext(shared_ptr<IDispenser> next) override {
        nextDispenser = next;
    }

    // Calculates notes and delegates remainder.
    void dispense(int amount, map<int, int>& result) override {
        if (amount >= denomination) {
            int numNotes = amount / denomination;
            int remainder = amount % denomination;
            
            result[denomination] = numNotes; 
            amount = remainder;              
        }
        
        if (amount > 0 && nextDispenser) {
            nextDispenser->dispense(amount, result);
        } 
        else if (amount > 0) {
            throw runtime_error("Error: Cannot dispense exact amount.");
        }
    }
};

// Handles presentation/output.
class ATMLogger {
public:
    // Prints formatted receipt.
    static void printReceipt(const map<int, int>& notes) {
        cout << "--- WITHDRAWAL SUCCESSFUL ---\n";
        for (auto it = notes.rbegin(); it != notes.rend(); ++it) {
            cout << "₹" << it->first << " x " << it->second << " notes\n";
        }
    }
};

// Application entry point.
int main() {
    auto d2000 = make_shared<CashDispenser>(2000);
    auto d500  = make_shared<CashDispenser>(500);
    auto d100  = make_shared<CashDispenser>(100);

    d2000->setNext(d500);
    d500->setNext(d100);

    int amount = 2600;
    map<int, int> result;

    try {
        d2000->dispense(amount, result);
        ATMLogger::printReceipt(result);
    } catch (const exception& e) {
        cout << e.what() << endl;
    }

    return 0;
}