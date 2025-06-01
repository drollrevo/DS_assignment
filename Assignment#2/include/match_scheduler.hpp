#pragma once
#include <string>

// Player structure for match scheduling
struct Player {
    std::string playerID;
    std::string playerName;
    std::string teamID;
    int rank;
    int wins;
    int losses;
    std::string currentStage; // "qualifier", "group", "knockout", "eliminated"
    bool isActive;
    int arrivalOrder;
    
    Player() : rank(0), wins(0), losses(0), currentStage("qualifier"), isActive(true), arrivalOrder(0) {}
};

// Match structure
struct Match {
    std::string matchID;
    std::string teamA;
    std::string teamB;
    int scoreA;
    int scoreB;
    std::string winner;
    std::string timestamp;
    std::string status; // "scheduled", "ongoing", "completed"
    
    Match() : scoreA(0), scoreB(0), status("scheduled") {}
};

// Node for player priority queue (min-heap based on rank)
struct PlayerNode {
    Player* player;
    PlayerNode* next;
    
    PlayerNode(Player* p) : player(p), next(nullptr) {}
};

// Node for match queue
struct MatchNode {
    Match* match;
    MatchNode* next;
    
    MatchNode(Match* m) : match(m), next(nullptr) {}
};

class MatchScheduler {
private:
    Player** playerHeap;    // Dynamic array for player heap
    int heapSize;
    int heapCapacity;
    int playerCount;
    
    MatchNode* matchQueueHead;
    MatchNode* matchQueueTail;
    
    Player* allPlayers;     // Array to store all players
    Match* completedMatches; // Array to store completed matches
    int maxPlayers;
    int completedCount;
    
    // Heap operations
    void siftUp(int idx);
    void siftDown(int idx);
    void resizeHeap();
    int comparePlayer(Player* a, Player* b);
    
    // Helper functions
    void parseLine(const std::string& line, std::string* fields, int expectedFields);
    
public:
    MatchScheduler();
    ~MatchScheduler();
    
    // Load data from CSV files
    void loadTeams(const std::string& filename);
    void loadPlayers(const std::string& filename);
    void loadResults(const std::string& filename);
    
    // Core functionality
    void generateMatchPairings(const std::string& stage);
    Match* getNextMatch();
    void recordMatchResult(const std::string& matchID, const std::string& winner);
    void updatePlayerStats();
    
    // Display functions
    void displayTournamentBracket();
    void displayMatchSchedule();
    void displayPlayerStats();
    
    // Queue management
    bool isEmpty() const;
    int size() const;
    void clearAll();
};