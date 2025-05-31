#include "spectator_queue.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

static const int INITIAL_CAPACITY = 16;

SpectatorQueue::SpectatorQueue()
    : heapSize(0), heapCapacity(INITIAL_CAPACITY), arrivalCounter(0), slotsHead(nullptr), admitLog(nullptr)
{
    heap = new Viewer*[heapCapacity];
    // Open admission log in append mode (or create if it doesn't exist)
    admitLog = std::fopen("data/admitted_viewers.csv", "a");
    if (!admitLog) {
        std::perror("Error opening admitted_viewers.csv");
        std::exit(EXIT_FAILURE);
    }
    // If the file is brand-new (size 0), write header
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
    // Free the linked list of slots
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
    // Skip header row
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
    // Skip header row
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

    // Log admission (placeholder admit time & channel)
    std::string admitTime = "2025-06-15T00:00";  // Replace with real timestamp if desired
    std::fprintf(admitLog, "%s,%s,%d,%s,%s\n",
                 top->id.c_str(), top->name.c_str(), top->priority,
                 admitTime.c_str(), "ChannelX");
    std::fflush(admitLog);
    return top;
}

bool SpectatorQueue::isEmpty() const {
    return heapSize == 0;
}

int SpectatorQueue::size() const {
    return heapSize;
}

void SpectatorQueue::clearAll() {
    // Delete all remaining Viewer* in the heap
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
            std::swap(heap[idx], heap[parent]);
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
            std::swap(heap[idx], heap[largest]);
            idx = largest;
        } else {
            break;
        }
    }
}

int SpectatorQueue::compare(Viewer* a, Viewer* b) const {
    // Lower `priority` value = higher actual priority (1 is highest)
    if (a->priority < b->priority) return 1;
    if (a->priority > b->priority) return -1;
    // Tie-break by arrivalOrder (lower = earlier)
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
