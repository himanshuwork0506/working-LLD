1. Code Flow
Initialize Cache Limit
Request Cache Data
Promote Used Node
Evict LRU Node
Insert New Data

2. Classes
Node: Data Container
DoublyLinkedList: Chronological Order
LRUCache: Eviction Manager

3. Requirements
Functional:
Retrieve Cached Items
Insert New Items
Evict Oldest Data

Non-Functional:
O(1) Time Complexity
Memory Efficient

C++
#include <iostream>
#include <unordered_map>

using namespace std;

// Holds individual cache entries and navigational pointers for the linked list.
struct Node {
    int key, val;
    Node* prev;
    Node* next;
    
    Node(int k, int v) : key(k), val(v), prev(nullptr), next(nullptr) {}
};

// Manages chronological order of access using O(1) additions and removals.
struct DoublyLinkedList {
    Node* head;
    Node* tail;

    DoublyLinkedList() {
        // Dummy head and tail eliminate the need for null checks during insertion/deletion.
        head = new Node(-1, -1); 
        tail = new Node(-1, -1); 
        head->next = tail;
        tail->prev = head;
    }

    // Inserts a node immediately after the head, marking it as the Most Recently Used.
    void addFront(Node* node) {
        Node* temp = head->next;
        node->next = temp;
        node->prev = head;
        head->next = node;
        temp->prev = node;
    }

    // Severs a node's connections to instantly extract it from the list.
    void removeNode(Node* node) {
        Node* delPrev = node->prev;
        Node* delNext = node->next;
        delPrev->next = delNext;
        delNext->prev = delPrev;
    }
};

// Orchestrates the LRU caching strategy using a hash map and doubly linked list.
class LRUCache {
private:
    int capacity;
    // Maps a key directly to its memory address in the linked list for O(1) access.
    unordered_map<int, Node*> cacheMap; 
    DoublyLinkedList list;

    // Helper to upgrade a node to Most Recently Used status by moving it to the front.
    void moveToFront(Node* node) {
        list.removeNode(node);
        list.addFront(node);
    }

public:
    LRUCache(int cap) : capacity(cap) {}

    // Retrieves a value by key and promotes it to the front of the cache.
    int get(int key) {
        if (cacheMap.find(key) == cacheMap.end()) return -1;
        
        Node* node = cacheMap[key];
        moveToFront(node); 
        return node->val;
    }

    // Inserts or updates a key-value pair, triggering an eviction if at maximum capacity.
    void put(int key, int value) {
        // SCENARIO 1: Key already exists. Update value and promote to front.
        if (cacheMap.find(key) != cacheMap.end()) {
            Node* node = cacheMap[key];
            node->val = value;
            moveToFront(node);
            return;
        }

        // SCENARIO 2: Cache is full. Evict the Least Recently Used node.
        if (cacheMap.size() == capacity) {
            // The node right before the dummy tail is always the LRU.
            Node* lruNode = list.tail->prev; 
            
            cacheMap.erase(lruNode->key); 
            list.removeNode(lruNode); 
            delete lruNode; 
        }

        // SCENARIO 3: Insert brand new key at the front.
        Node* newNode = new Node(key, value);
        list.addFront(newNode);
        cacheMap[key] = newNode; 
    }
};

// Application entry point demonstrating LRU eviction.
int main() {
    // 1. Initialize Cache Limit
    LRUCache cache(2);

    // 5. Insert New Data
    cache.put(1, 10);
    cache.put(2, 20);
    
    // 2. Request Cache Data & 3. Promote Used Node
    cout << "Get 1: " << cache.get(1) << "\n"; // Returns 10, moves 1 to front

    // 4. Evict LRU Node
    cache.put(3, 30); // Evicts key 2
    cout << "Get 2: " << cache.get(2) << "\n"; // Returns -1 (not found)

    cache.put(4, 40); // Evicts key 1
    cout << "Get 1: " << cache.get(1) << "\n"; // Returns -1 (not found)
    cout << "Get 3: " << cache.get(3) << "\n"; // Returns 30
    cout << "Get 4: " << cache.get(4) << "\n"; // Returns 40

    return 0;
}