#pragma once
#include <string>

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
    std::string teamA;
    std::string teamB;
    int scoreA;
    int scoreB;
    std::string winner;
    std::string timestamp;
    std::string status;
    std::string round;
    
    Match() : scoreA(0), scoreB(0), status("scheduled"), round("") {}
};

// Node for match queue
struct MatchNode {
    Match* match;
    MatchNode* next;
    
    MatchNode(Match* m) : match(m), next(nullptr) {}
};

// Structure to hold match result details for performance history
struct MatchResult {
    int matchId;
    std::string team1;
    std::string team2;
    std::string winner;
    std::string loser;
    int team1Score;
    int team2Score;
    std::string gameMode;
    std::string matchDate;
    int durationMinutes;
};

class PerformanceHistoryManager {
public:
    virtual void recordMatchResult(const MatchResult& result) = 0;
    virtual void displayAllPlayerStats() = 0;
    virtual void displayRecentMatches() = 0;
    virtual void searchPlayerMatches(const std::string& playerName) = 0;
    virtual void getTopPlayersByWinRate(int n) = 0;
    virtual void getTopPlayersByScore(int n) = 0;
    virtual void generateTournamentSummary() = 0;
    virtual ~PerformanceHistoryManager() {}
};

class MatchScheduler {
private:
    PerformanceHistoryManager* perfManager;
    Player** playerHeap;
    int heapSize;
    int heapCapacity;
    int teamCount;
    MatchNode* matchQueueHead;
    MatchNode* matchQueueTail;
    Player* allTeams;
    Match* completedMatches;
    int completedCount;
    int maxTeams;
    int teamsRemaining;
    std::string currentRound;
    
    void siftUp(int idx);
    void siftDown(int idx);
    void resizeHeap();
    int comparePlayer(Player* a, Player* b);
    void parseLine(const std::string& line, std::string* fields, int expectedFields);
    void saveMatchResult(const std::string& matchID, const std::string& winner, int scoreA, int scoreB, const std::string& teamA, const std::string& teamB);
    std::string getTeamName(const std::string& teamID);
    void sortTeamsByRank();
    void clearMatchQueue();
    void displayBracketVisual(Player** teams, int teamCount);

public:
    MatchScheduler(PerformanceHistoryManager* perfManager = nullptr);
    ~MatchScheduler();
    
    void loadTeams(const std::string& filename);
    void loadResults(const std::string& filename);
    
    void generateMatchPairings();
    Match* getNextMatch();
    void recordMatchResult(const std::string& matchID, const std::string& winner);
    
    void displayTournamentBracket();
    void displayMatchSchedule();
    void displayPlayerStats();
    
    bool isEmpty() const;
    int size() const;
    void clearAll();
};