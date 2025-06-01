#pragma once
#include "team.hpp"
#include <string>

// Node for priority queue implementation
struct PriorityNode {
    Team* team;
    PriorityNode* next;
    
    PriorityNode(Team* t) : team(t), next(nullptr) {}
};

// Node for regular queue implementation
struct QueueNode {
    Team* team;
    QueueNode* next;
    
    QueueNode(Team* t) : team(t), next(nullptr) {}
};

class TournamentRegistration {
public:
    TournamentRegistration();
    ~TournamentRegistration();
    
    // Load teams from CSV
    void loadTeams(const std::string& filename);
    
    // Registration operations
    bool registerTeam(const std::string& teamID, const std::string& regType);
    void processRegistrations();
    
    // Check-in operations
    bool checkInTeam(const std::string& teamID);
    void displayCheckInQueue();
    
    // Withdrawal operations
    void withdrawTeam(const std::string& teamID);
    void addReplacementTeam(const std::string& teamID);
    
    // Display functions
    void displayStats();
    void displayRegistrationQueue();
    
    bool isEmpty() const;
    int getRegisteredCount() const;

private:
    // Priority queue for registrations (early-bird > wildcard > regular)
    PriorityNode* regHead;
    
    // Regular queue for check-ins (FIFO)
    QueueNode* checkInFront;
    QueueNode* checkInRear;
    
    // Regular queue for withdrawals
    QueueNode* withdrawalFront;
    QueueNode* withdrawalRear;
    
    // Array to store all teams loaded from CSV
    Team* allTeams;
    int teamCount;
    int teamCapacity;
    
    // Array to store registered teams
    Team** registeredTeams;
    int registeredCount;
    int registeredCapacity;
    
    int arrivalCounter;
    static const int MAX_CAPACITY = 32; // Tournament capacity
    
    // Helper functions
    void insertPriorityQueue(Team* team);
    Team* dequeuePriorityQueue();
    void enqueueRegular(QueueNode*& front, QueueNode*& rear, Team* team);
    Team* dequeueRegular(QueueNode*& front, QueueNode*& rear);
    void clearQueue(QueueNode*& front, QueueNode*& rear);
    void clearPriorityQueue();
    
    Team* findTeamByID(const std::string& teamID);
    int getPriority(const std::string& regType);
    void parseLine(const std::string& line, std::string* fields, int expectedFields);
    void resizeTeamArray();
    void resizeRegisteredArray();
};
