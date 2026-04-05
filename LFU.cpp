Gemini said
1. Code Flow
Initialize Cache Capacity
Request Cache Data
Update Access Frequency
Evict Lowest Frequency
Insert New Data

2. Classes
Node: Data Container
DoublyLinkedList: Frequency Bucket
LFUCache: Eviction Manager

3. Requirements
Functional:
Retrieve Cached Items
Insert New Items
Evict LFU/LRU

Non-Functional:
O(1) Time Complexity
Memory Efficient

C++
#include <iostream>
#include <unordered_map>

using namespace std;

// Holds individual cache entries, their current access frequency, and navigational pointers.
struct Node {
    int key, val, freq;
    Node* prev;
    Node* next;
    
    Node(int k, int v) : key(k), val(v), freq(1), prev(nullptr), next(nullptr) {}
};

// Manages nodes of the exact same frequency, enabling O(1) additions and removals.
struct DoublyLinkedList {
    int size;
    Node* head;
    Node* tail;

    DoublyLinkedList() {
        size = 0;
        head = new Node(-1, -1); 
        tail = new Node(-1, -1); 
        head->next = tail;
        tail->prev = head;
    }

    // Inserts a node immediately after the head, marking it as the Most Recently Used in this frequency bucket.
    void addFront(Node* node) {
        Node* temp = head->next;
        node->next = temp;
        node->prev = head;
        head->next = node;
        temp->prev = node;
        size++;
    }

    // Severs a node's connections to instantly extract it from the list.
    void removeNode(Node* node) {
        Node* delPrev = node->prev;
        Node* delNext = node->next;
        delPrev->next = delNext;
        delNext->prev = delPrev;
        size--;
    }
};

// Orchestrates the LFU caching strategy using a combination of hash maps and doubly linked lists to guarantee O(1) operations.
class LFUCache {
private:
    int capacity;
    int minFreq;
    int currentSize;

    unordered_map<int, Node*> keyNode; 
    
    unordered_map<int, DoublyLinkedList*> freqMap; 

    // Moves a accessed node from its current frequency bucket to the next higher frequency bucket.
    void updateFreq(Node* node) {
        freqMap[node->freq]->removeNode(node);
        
        if (node->freq == minFreq && freqMap[node->freq]->size == 0) {
            minFreq++;
        }

        node->freq++;

        if (freqMap.find(node->freq) == freqMap.end()) {
            freqMap[node->freq] = new DoublyLinkedList();
        }
        
        freqMap[node->freq]->addFront(node);
    }

public:
    LFUCache(int cap) : capacity(cap), minFreq(0), currentSize(0) {}

    // Retrieves a value by key and increments its access frequency score.
    int get(int key) {
        if (keyNode.find(key) == keyNode.end()) return -1;
        
        Node* node = keyNode[key];
        updateFreq(node); 
        return node->val;
    }

    // Inserts or updates a key-value pair, triggering an eviction if the cache is at maximum capacity.
    void put(int key, int value) {
        if (capacity == 0) return;

        if (keyNode.find(key) != keyNode.end()) {
            Node* node = keyNode[key];
            node->val = value;
            updateFreq(node);
            return;
        }

        if (currentSize == capacity) {
            DoublyLinkedList* minList = freqMap[minFreq];
            Node* lruNode = minList->tail->prev; 
            
            keyNode.erase(lruNode->key); 
            minList->removeNode(lruNode); 
            delete lruNode; 
            currentSize--;
        }

        currentSize++;
        minFreq = 1;
        
        if (freqMap.find(1) == freqMap.end()) {
            freqMap[1] = new DoublyLinkedList();
        }
        
        Node* newNode = new Node(key, value);
        freqMap[1]->addFront(newNode);
        keyNode[key] = newNode; 
    }
};