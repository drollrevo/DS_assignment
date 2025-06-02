#pragma once
#include "team.hpp"
#include <string>
#include <cstdio>

// Circular Queue Node for Check-ins
struct CircularQueueNode {
    Team* team;
    int next;
    CircularQueueNode() : team(nullptr), next(-1) {}
};

// Stack Node for Withdrawals/Replacements
struct StackNode {
    Team* team;
    StackNode* next;
    StackNode(Team* t) : team(t), next(nullptr) {}
};

// Priority Queue Node for Registrations
struct PriorityNode {
    Team* team;
    PriorityNode* next;
    PriorityNode(Team* t) : team(t), next(nullptr) {}
};

class TournamentRegistration {
public:
    TournamentRegistration();
    ~TournamentRegistration();
    
    // Load teams from CSV
    void loadTeams(const std::string& filename);
    void saveRegistrationLog();
    
    // Registration operations
    bool registerTeam(const std::string& teamID, const std::string& regType);
    void processRegistrations();
    
    // Check-in operations (Circular Queue)
    bool checkInTeam(const std::string& teamID);
    void displayCheckInQueue();
    Team* getNextCheckedInTeam();
    
    // Withdrawal operations (Stack)
    void withdrawTeam(const std::string& teamID);
    void addReplacementTeam(const std::string& teamID);
    Team* popWithdrawal();
    void displayWithdrawalStack();
    
    // Display functions
    void displayStats();
    void displayRegistrationQueue();
    void displayMenu();
    void handleUserInput(int choice);
    
    bool isEmpty() const;
    int getRegisteredCount() const;

private:
    // Priority queue for registrations (early-bird > wildcard > regular)
    PriorityNode* regHead;
    
    // Circular Queue for check-ins - reduced to 16 for tournament limit
    static const int CIRCULAR_SIZE = 16;
    CircularQueueNode circularQueue[CIRCULAR_SIZE];
    int front, rear, queueCount;
    
    // Stack for withdrawals
    StackNode* withdrawalTop;
    
    // Array to store all teams loaded from CSV
    Team* allTeams;
    int teamCount;
    int teamCapacity;
    
    // Array to store registered teams
    Team** registeredTeams;
    int registeredCount;
    int registeredCapacity;
    
    int arrivalCounter;
    static const int MAX_CAPACITY = 16; // Fixed to 16 teams max
    
    // File pointer for logging
    std::FILE* regLog;
    
    // Helper functions
    void insertPriorityQueue(Team* team);
    Team* dequeuePriorityQueue();
    
    // Circular Queue operations
    bool isCircularQueueFull();
    bool isCircularQueueEmpty();
    void enqueueCircular(Team* team);
    Team* dequeueCircular();
    
    // Stack operations
    void pushWithdrawal(Team* team);
    bool isStackEmpty();
    
    void clearPriorityQueue();
    void clearWithdrawalStack();
    
    Team* findTeamByID(const std::string& teamID);
    int getPriority(const std::string& regType);
    void parseLine(const std::string& line, std::string* fields, int expectedFields);
    void resizeTeamArray();
    void resizeRegisteredArray();
};
