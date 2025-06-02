#include "spectator_queue.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>

static const int INITIAL_CAPACITY = 16;

SpectatorQueue::SpectatorQueue()
    : heapSize(0), heapCapacity(INITIAL_CAPACITY), arrivalCounter(0), slotsHead(nullptr), admitLog(nullptr)
{
    heap = new Viewer*[heapCapacity];
    admitLog = std::fopen("data/admitted_viewers.csv", "a");
    if (!admitLog) {
        std::perror("Error opening admitted_viewers.csv");
        std::exit(EXIT_FAILURE);
    }
    std::fseek(admitLog, 0, SEEK_END);
    long pos = std::ftell(admitLog);
    if (pos == 0) {
        std::fprintf(admitLog, "ViewerID,ViewerName,Priority,AdmitTime,ChannelID\n");
        std::fflush(admitLog);
    }
}

SpectatorQueue::~SpectatorQueue() {
    clearAll();
    delete[] heap;
    StreamSlot* cur = slotsHead;
    while (cur) {
        StreamSlot* tmp = cur->next;
        delete cur;
        cur = tmp;
    }
    if (admitLog) std::fclose(admitLog);
}

void SpectatorQueue::loadViewers(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::string line;
    if (!std::getline(file, line)) return;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string fields[3];
        parseLine(line, fields, 3);
        Viewer* v = new Viewer;
        v->id = fields[0];
        v->name = fields[1];
        v->priority = std::stoi(fields[2]);
        v->arrivalOrder = ++arrivalCounter;
        enqueueViewer(v);
    }
    file.close();
}

void SpectatorQueue::loadSlots(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::string line;
    if (!std::getline(file, line)) return;
    StreamSlot* tail = nullptr;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::string fields[4];
        parseLine(line, fields, 4);
        StreamSlot* slot = new StreamSlot;
        slot->slotID = fields[0];
        slot->timeStart = fields[1];
        slot->timeEnd = fields[2];
        slot->channelID = fields[3];
        slot->next = nullptr;

        if (!slotsHead) {
            slotsHead = slot;
            tail = slot;
        } else {
            tail->next = slot;
            tail = slot;
        }
    }
    file.close();
}

void SpectatorQueue::enqueueViewer(Viewer* v) {
    if (v->arrivalOrder == 0) {
        v->arrivalOrder = ++arrivalCounter;
    }
    if (heapSize == heapCapacity) {
        resizeHeap();
    }
    heap[heapSize] = v;
    siftUp(heapSize);
    heapSize++;
}

Viewer* SpectatorQueue::dequeueViewer() {
    if (heapSize == 0) return nullptr;
    Viewer* top = heap[0];
    heapSize--;
    heap[0] = heap[heapSize];
    siftDown(0);

    std::string admitTime = "2025-06-15T00:00";
    std::fprintf(admitLog, "%s,%s,%d,%s,%s\n",
                 top->id.c_str(), top->name.c_str(), top->priority,
                 admitTime.c_str(), "ChannelX");
    std::fflush(admitLog);
    return top;
}

Viewer* SpectatorQueue::peekNext() const {
    if (heapSize == 0) return nullptr;
    return heap[0];
}

void SpectatorQueue::displayQueue() const {
    if (heapSize == 0) {
        std::cout << "No spectators are currently in the queue.\n";
        return;
    }
    
    Viewer** temp = new Viewer*[heapSize];
    for (int i = 0; i < heapSize; i++) {
        temp[i] = heap[i];
    }
    
    // Bubble sort
    for (int i = 0; i < heapSize - 1; i++) {
        for (int j = 0; j < heapSize - i - 1; j++) {
            if (compare(temp[j + 1], temp[j]) > 0) {
                Viewer* swap = temp[j];
                temp[j] = temp[j + 1];
                temp[j + 1] = swap;
            }
        }
    }
    
    std::cout << "\n=== Spectator Queue (Priority Order) ===\n";
    std::cout << std::left << std::setw(5) << "No." 
              << std::setw(20) << "Name" 
              << std::setw(10) << "ID" 
              << std::setw(10) << "Priority" 
              << std::setw(10) << "Arrival" << "\n";
    std::cout << "----------------------------------------\n";
    for (int i = 0; i < heapSize; i++) {
        Viewer* v = temp[i];
        std::cout << std::left << std::setw(5) << (i + 1) 
                  << std::setw(20) << v->name 
                  << std::setw(10) << v->id 
                  << std::setw(10) << v->priority 
                  << std::setw(10) << v->arrivalOrder << "\n";
    }
    std::cout << "========================================\n";
    
    delete[] temp;
}

void SpectatorQueue::displaySlots() const {
    if (!slotsHead) {
        std::cout << "No stream slots are currently available.\n";
        return;
    }
    
    std::cout << "\n=== Available Stream Slots ===\n";
    std::cout << std::left << std::setw(5) << "No." 
              << std::setw(10) << "Slot ID" 
              << std::setw(25) << "Time" 
              << std::setw(15) << "Channel" << "\n";
    StreamSlot* current = slotsHead;
    int count = 0;
    while (current) {
        count++;
        std::string time = current->timeStart + " - " + current->timeEnd;
        std::cout << std::left << std::setw(5) << count 
                  << std::setw(10) << current->slotID 
                  << std::setw(25) << time 
                  << std::setw(15) << current->channelID << "\n";
        current = current->next;
    }
    std::cout << "Total Slots: " << count << "\n";
    std::cout << "==============================\n";
}

bool SpectatorQueue::isEmpty() const {
    return heapSize == 0;
}

int SpectatorQueue::size() const {
    return heapSize;
}

void SpectatorQueue::clearAll() {
    for (int i = 0; i < heapSize; i++) {
        delete heap[i];
    }
    heapSize = 0;
}

void SpectatorQueue::resizeHeap() {
    int newCap = heapCapacity * 2;
    Viewer** newArr = new Viewer*[newCap];
    for (int i = 0; i < heapSize; i++) {
        newArr[i] = heap[i];
    }
    delete[] heap;
    heap = newArr;
    heapCapacity = newCap;
}

void SpectatorQueue::siftUp(int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (compare(heap[idx], heap[parent]) > 0) {
            Viewer* temp = heap[idx];
            heap[idx] = heap[parent];
            heap[parent] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}

void SpectatorQueue::siftDown(int idx) {
    while (true) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int largest = idx;
        if (left < heapSize && compare(heap[left], heap[largest]) > 0) {
            largest = left;
        }
        if (right < heapSize && compare(heap[right], heap[largest]) > 0) {
            largest = right;
        }
        if (largest != idx) {
            Viewer* temp = heap[idx];
            heap[idx] = heap[largest];
            heap[largest] = temp;
            idx = largest;
        } else {
            break;
        }
    }
}

int SpectatorQueue::compare(Viewer* a, Viewer* b) const {
    if (a->priority < b->priority) return 1;
    if (a->priority > b->priority) return -1;
    if (a->arrivalOrder < b->arrivalOrder) return 1;
    if (a->arrivalOrder > b->arrivalOrder) return -1;
    return 0;
}

void SpectatorQueue::parseLine(const std::string& line, std::string* fields, int expectedFields) {
    std::stringstream ss(line);
    std::string item;
    int idx = 0;
    while (std::getline(ss, item, ',') && idx < expectedFields) {
        fields[idx++] = item;
    }
    if (idx != expectedFields) {
        std::cerr << "Malformed CSV line: " << line << std::endl;
        std::exit(EXIT_FAILURE);
    }
}