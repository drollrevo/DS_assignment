#pragma once
#include <string>
#include "task4.hpp" // For PerformanceHistoryManager

struct Player {
    std::string playerID;
    std::string playerName;
    std::string teamID;
    int rank;
    int wins;
    int losses;
    int totalScore;
    std::string currentStage;
    bool isActive;
    int arrivalOrder;
    
    Player() : rank(0), wins(0), losses(0), totalScore(0), currentStage("active"), isActive(true), arrivalOrder(0) {}
};

struct Match {
    std::string matchID;
    std::string playerA;
    std::string playerB;
    int scoreA;
    int scoreB;
    std::string winner;
    std::string timestamp;
    std::string status;
    std::string round;
    
    Match() : scoreA(0), scoreB(0), status("scheduled"), round("") {}
};

struct MatchNode {
    Match* match;
    MatchNode* next;
    
    MatchNode(Match* m) : match(m), next(nullptr) {}
};

class MatchScheduler {
private:
    Player** playerHeap;
    int heapSize;
    int heapCapacity;
    int playerCount;
    MatchNode* matchQueueHead;
    MatchNode* matchQueueTail;
    Player* allPlayers;
    Match* completedMatches;
    int completedCount;
    int maxPlayers;
    std::string currentRound;
    PerformanceHistoryManager* perfManager;

    void siftUp(int idx);
    void siftDown(int idx);
    void resizeHeap();
    int comparePlayer(Player* a, Player* b);
    void parseLine(const std::string& line, std::string* fields, int expectedFields);
    void generateCurrentRoundMatches();
    void saveMatchResult(const std::string& matchID, const std::string& winner, int scoreA, int scoreB);
    std::string getPlayerName(const std::string& playerID);
    void sortPlayersByRank();
    void clearMatchQueue();

public:
    MatchScheduler(PerformanceHistoryManager* perfManager);
    ~MatchScheduler();
    void loadPlayers(const std::string& filename);
    void loadResults(const std::string& filename);
    void generateMatchPairings(const std::string& stage);
    Match* getNextMatch();
    void recordMatchResult(const std::string& matchID, const std::string& winner);
    void displayTournamentBracket();
    void displayMatchSchedule();
    void displayPlayerStats();
    bool isEmpty() const;
    int size() const;
    void clearAll();
};