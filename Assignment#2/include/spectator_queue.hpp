#pragma once
#include "viewer.hpp"
#include <string>
#include <cstdio>

// Simple linked-list node for stream slots
struct StreamSlot {
    std::string slotID;
    std::string timeStart;
    std::string timeEnd;
    std::string channelID;
    StreamSlot* next;
};

class SpectatorQueue {
public:
    SpectatorQueue();
    ~SpectatorQueue();

    // Load viewers from CSV (viewers.csv in data/)
    void loadViewers(const std::string& filename);
    // Load slots from CSV (stream_slots.csv in data/)
    void loadSlots(const std::string& filename);

    // Enqueue a single viewer at runtime
    void enqueueViewer(Viewer* v);
    // Dequeue the highest-priority viewer (or nullptr if empty)
    Viewer* dequeueViewer();

    bool isEmpty() const;
    int size() const;
    // Clear all remaining viewers and free memory
    void clearAll();

private:
    Viewer** heap;      // Binary heap array of Viewer* pointers
    int heapSize;
    int heapCapacity;
    int arrivalCounter; // Next arrivalOrder value when loading

    // Dynamic resizing of the heap array
    void resizeHeap();
    // Restore heap upward from idx
    void siftUp(int idx);
    // Restore heap downward from idx
    void siftDown(int idx);
    // Compare two viewers: return positive if a> b (higher priority), negative if a < b
    int compare(Viewer* a, Viewer* b) const;

    // Linked list of stream slots
    StreamSlot* slotsHead;

    // File pointer for admission log (admitted_viewers.csv)
    std::FILE* admitLog;

    // Helper to parse CSV line into fields
    void parseLine(const std::string& line, std::string* fields, int expectedFields);
};
