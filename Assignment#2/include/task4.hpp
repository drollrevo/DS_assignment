#ifndef TASK4_HPP
#define TASK4_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <cstring> // Add this for C string functions

// ==================== MatchResult Structure ====================
// Structure to represent a match result
class MatchResult {
public:
    int matchId;
    std::string player1;
    std::string player2;
    std::string winner;
    std::string loser;
    int player1Score;
    int player2Score;
    std::string gameMode;
    std::string matchDate;
    int durationMinutes;

    // Default constructor
    MatchResult();

    // Parameterized constructor
    MatchResult(int id, const std::string& p1, const std::string& p2,
                const std::string& win, const std::string& lose,
                int score1, int score2, const std::string& mode,
                const std::string& date, int duration);

    // Display match result
    void displayMatch() const;
};

// ==================== PlayerStats Structure ====================
// Structure to represent player performance statistics
class PlayerStats {
public:
    std::string playerName;
    int totalMatches;
    int wins;
    int losses;
    int totalScore;
    double winRate;
    double averageScore;

    // Default constructor
    PlayerStats();

    // Parameterized constructor
    PlayerStats(const std::string& name);

    // Update stats after a match
    void updateStats(bool won, int score);

    // Calculate win rate
    void calculateWinRate();

    // Calculate average score
    void calculateAverageScore();

    // Display player statistics
    void displayStats() const;
};

// ==================== RecentMatchesQueue Class (Circular Queue) ====================
// Circular Queue class for managing recent match results
class RecentMatchesQueue {
private:
    MatchResult* matches;
    int front;
    int rear;
    int capacity;
    int count;

public:
    // Constructor
    RecentMatchesQueue(int size = 20);

    // Destructor
    ~RecentMatchesQueue();

    // Check if queue is empty
    bool isEmpty() const;

    // Check if queue is full
    bool isFull() const;

    // Add a new match result (enqueue)
    void addMatch(const MatchResult& match);

    // Remove oldest match result (dequeue)
    MatchResult removeOldestMatch();

    // Get the most recent match without removing it
    MatchResult getMostRecentMatch() const;

    // Display all recent matches
    void displayRecentMatches() const;

    // Get current number of matches stored
    int getCount() const;

    // Search for matches by player name
    void searchMatchesByPlayer(const std::string& playerName) const;

    // Get matches by game mode
    void getMatchesByGameMode(const std::string& gameMode) const;

    // Get matches by date (enhanced feature)
    void getMatchesByDate(const std::string& date) const;
};

// ==================== PerformanceHistoryManager Class ====================
// Performance History Management System
class PerformanceHistoryManager {
private:
    PlayerStats* playerDatabase;
    int maxPlayers;
    int currentPlayerCount;
    RecentMatchesQueue* recentMatches;

    // Helper function to find player index
    int findPlayerIndex(const std::string& playerName);

public:
    // Constructor
    PerformanceHistoryManager(int maxPlayers = 100, int recentMatchesSize = 20);

    // Destructor
    ~PerformanceHistoryManager();

    // Core functionality
    void recordMatchResult(const MatchResult& match);
    PlayerStats getPlayerStats(const std::string& playerName);
    void displayAllPlayerStats() const;
    void displayRecentMatches() const;

    // Analysis and reporting
    void getTopPlayersByWinRate(int topN = 5) const;
    void getTopPlayersByScore(int topN = 5) const;
    void searchPlayerMatches(const std::string& playerName) const;
    void generateTournamentSummary() const;
    void generatePerformanceReport() const;

    // Data management and CSV integration
    void exportPlayerStatsToFile(const std::string& filename) const;
    void loadMatchResultsFromCSV(const std::string& filename);
    void loadPlayersFromCSV(const std::string& filename);
    int getTotalMatchesRecorded() const;
    void clearAllData();

    // Additional analysis features
    void getPlayerHeadToHead(const std::string& player1, const std::string& player2) const;
    void getGameModeStatistics() const;
    void getMatchesByDateRange(const std::string& startDate, const std::string& endDate) const;
};

// ==================== Utility Functions ====================
char** splitCSVLine(const std::string& line, int& size);
bool isValidDate(const std::string& date);
std::string getCurrentDate();

#endif // TASK4_HPP
