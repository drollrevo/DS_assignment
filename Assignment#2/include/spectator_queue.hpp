#pragma once
#include "viewer.hpp"
#include <string>
#include <cstdio>

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
    
    void loadViewers(const std::string& filename);
    void loadSlots(const std::string& filename);
    
    void enqueueViewer(Viewer* v);
    Viewer* dequeueViewer();
    Viewer* peekNext() const;
    
    void displayQueue() const;
    void displaySlots() const;
    
    bool isEmpty() const;
    int size() const;
    void clearAll();

private:
    Viewer** heap;
    int heapSize;
    int heapCapacity;
    int arrivalCounter;
    
    StreamSlot* slotsHead;
    std::FILE* admitLog;
    
    void resizeHeap();
    void siftUp(int idx);
    void siftDown(int idx);
    int compare(Viewer* a, Viewer* b) const;
    void parseLine(const std::string& line, std::string* fields, int expectedFields);
};