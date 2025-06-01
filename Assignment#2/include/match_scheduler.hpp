#pragma once
#include <string>

struct Player {
    std::string playerID;
    std::string playerName;
    std::string teamID;
    int rank;
    int wins;
    int losses;
    int totalScore; // Added to track total score
    std::string currentStage; // "qualifier", "group", "knockout", "eliminated"
    bool isActive;
    int arrivalOrder;
    
    Player() : rank(0), wins(0), losses(0), totalScore(0), currentStage("qualifier"), isActive(true), arrivalOrder(0) {}
};

struct Match {
    std::string matchID;
    std::string teamA;
    std::string teamB;
    int scoreA;
    int scoreB;
    std::string winner;
    std::string timestamp;
    std::string status; // "scheduled", "ongoing", "completed"
    std::string round; // Added to track the match round
    
    Match() : scoreA(0), scoreB(0), status("scheduled"), round("") {}
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
    int completedCount;
    int maxPlayers;
    int teamsRemaining;
    std::string currentRound;
    
    // Heap operations
    void siftUp(int idx);
    void siftDown(int idx);
    void resizeHeap();
    int comparePlayer(Player* a, Player* b);
    
    // Helper functions
    void parseLine(const std::string& line, std::string* fields, int expectedFields);
    void generateCurrentRoundMatches();
    void saveMatchResult(const std::string& matchID, const std::string& winner, int scoreA, int scoreB);
    std::string getTeamName(const std::string& teamID);
    void sortTeamsByRank();
    void clearMatchQueue();
    
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