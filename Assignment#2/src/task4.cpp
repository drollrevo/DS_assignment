#include "task4.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cstring> // Add this for C string functions

// Replace the splitCSVLine function with this improved version
char** splitCSVLine(const std::string& line, int& size) {
    // First count the number of commas
    size = 1;
    for(size_t i = 0; i < line.length(); i++) {
        if(line[i] == ',') size++;
    }
    
    // Allocate array of char pointers
    char** result = new char*[size];
    
    // Copy line to modifiable buffer
    char* buffer = new char[line.length() + 1];
    strcpy(buffer, line.c_str());
    
    // Split by commas
    int index = 0;
    char* token = strtok(buffer, ",");
    while(token != nullptr && index < size) {
        // Trim leading whitespace
        while(*token == ' ' || *token == '\t' || *token == '\r' || *token == '\n') 
            token++;
            
        // Trim trailing whitespace
        int len = strlen(token);
        while(len > 0 && (token[len-1] == ' ' || token[len-1] == '\t' || 
              token[len-1] == '\r' || token[len-1] == '\n')) {
            token[len-1] = '\0';
            len--;
        }
        
        // Allocate and copy trimmed token
        result[index] = new char[len + 1];
        strcpy(result[index], token);
        index++;
        
        token = strtok(nullptr, ",");
    }
    
    delete[] buffer;
    return result;
}

// Helper function to clean up splitCSVLine allocated memory
void cleanupCSVArray(char** array, int size) {
    for(int i = 0; i < size; i++) {
        delete[] array[i];
    }
    delete[] array;
}

// Modify CSV loading functions to use new splitCSVLine
void PerformanceHistoryManager::loadMatchResultsFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file " << filename << std::endl;
        return;
    }
    
    std::string line;
    bool isFirstLine = true;
    int loadedMatches = 0;
    
    while (std::getline(file, line)) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        
        int fieldCount;
        char** fields = splitCSVLine(line, fieldCount);
        
        if (fieldCount >= 8) { // Minimum required fields
            try {
                int matchId = std::stoi(fields[0]);
                std::string player1 = fields[1];
                std::string player2 = fields[2];
                int player1Score = std::stoi(fields[3]);
                int player2Score = std::stoi(fields[4]);
                std::string winner = (player1Score > player2Score) ? player1 : player2;
                std::string loser = (player1Score > player2Score) ? player2 : player1;
                std::string gameMode = fields[5];
                std::string matchDate = fields[6];
                int duration = fieldCount > 7 ? std::stoi(fields[7]) : 30;
                
                MatchResult match(matchId, player1, player2, winner, loser,
                                player1Score, player2Score, gameMode, matchDate, duration);
                
                recordMatchResult(match);
                loadedMatches++;
                
            } catch (const std::exception& e) {
                std::cout << "Warning: Skipping invalid line in CSV" << std::endl;
            }
        }
        
        cleanupCSVArray(fields, fieldCount);
    }
    
    file.close();
    std::cout << "Successfully loaded " << loadedMatches << " matches from " << filename << std::endl;
}

void PerformanceHistoryManager::loadPlayersFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not open file " << filename << std::endl;
        return;
    }
    
    std::string line;
    bool isFirstLine = true;
    int loadedPlayers = 0;
    
    while (std::getline(file, line) && currentPlayerCount < maxPlayers) {
        if (isFirstLine) {
            isFirstLine = false;
            continue;
        }
        
        int fieldCount;
        char** fields = splitCSVLine(line, fieldCount);
        
        if (fieldCount > 0) {
            std::string playerName = fields[0];
            
            if (findPlayerIndex(playerName) == -1) {
                playerDatabase[currentPlayerCount] = PlayerStats(playerName);
                currentPlayerCount++;
                loadedPlayers++;
            }
        }
        
        cleanupCSVArray(fields, fieldCount);
    }
    
    file.close();
    std::cout << "Successfully loaded " << loadedPlayers << " players from " << filename << std::endl;
}

// ==================== Utility Functions ====================

bool isValidDate(const std::string& date) {
    // Simple date validation (assumes format: YYYY-MM-DD or DD/MM/YYYY)
    return !date.empty() && date.length() >= 8;
}

std::string getCurrentDate() {
    return "2024-12-01"; // Placeholder - in real implementation, use system date
}

// ==================== MatchResult Implementation ====================

MatchResult::MatchResult() {
    matchId = 0;
    player1 = "";
    player2 = "";
    winner = "";
    loser = "";
    player1Score = 0;
    player2Score = 0;
    gameMode = "";
    matchDate = "";
    durationMinutes = 0;
}

MatchResult::MatchResult(int id, const std::string& p1, const std::string& p2,
                        const std::string& win, const std::string& lose,
                        int score1, int score2, const std::string& mode,
                        const std::string& date, int duration) {
    matchId = id;
    player1 = p1;
    player2 = p2;
    winner = win;
    loser = lose;
    player1Score = score1;
    player2Score = score2;
    gameMode = mode;
    matchDate = date;
    durationMinutes = duration;
}

void MatchResult::displayMatch() const {
    std::cout << "=== Match #" << matchId << " ===" << std::endl;
    std::cout << "Players: " << player1 << " vs " << player2 << std::endl;
    std::cout << "Score: " << player1Score << " - " << player2Score << std::endl;
    std::cout << "Winner: " << winner << std::endl;
    std::cout << "Game Mode: " << gameMode << std::endl;
    std::cout << "Date: " << matchDate << std::endl;
    std::cout << "Duration: " << durationMinutes << " minutes" << std::endl;
    std::cout << "================================" << std::endl;
}

// ==================== PlayerStats Implementation ====================

PlayerStats::PlayerStats() {
    playerName = "";
    totalMatches = 0;
    wins = 0;
    losses = 0;
    totalScore = 0;
    winRate = 0.0;
    averageScore = 0.0;
}

PlayerStats::PlayerStats(const std::string& name) {
    playerName = name;
    totalMatches = 0;
    wins = 0;
    losses = 0;
    totalScore = 0;
    winRate = 0.0;
    averageScore = 0.0;
}

void PlayerStats::updateStats(bool won, int score) {
    totalMatches++;
    totalScore += score;
    
    if (won) {
        wins++;
    } else {
        losses++;
    }
    
    calculateWinRate();
    calculateAverageScore();
}

void PlayerStats::calculateWinRate() {
    if (totalMatches > 0) {
        winRate = (double)wins / totalMatches * 100.0;
    } else {
        winRate = 0.0;
    }
}

void PlayerStats::calculateAverageScore() {
    if (totalMatches > 0) {
        averageScore = (double)totalScore / totalMatches;
    } else {
        averageScore = 0.0;
    }
}

void PlayerStats::displayStats() const {
    std::cout << "=== Player Statistics ===" << std::endl;
    std::cout << "Player: " << playerName << std::endl;
    std::cout << "Total Matches: " << totalMatches << std::endl;
    std::cout << "Wins: " << wins << std::endl;
    std::cout << "Losses: " << losses << std::endl;
    std::cout << "Total Score: " << totalScore << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Win Rate: " << winRate << "%" << std::endl;
    std::cout << "Average Score: " << averageScore << std::endl;
    std::cout << "=========================" << std::endl;
}

// ==================== RecentMatchesQueue Implementation ====================

RecentMatchesQueue::RecentMatchesQueue(int size) {
    capacity = size;
    matches = new MatchResult[capacity];
    front = 0;
    rear = -1;
    count = 0;
}

RecentMatchesQueue::~RecentMatchesQueue() {
    delete[] matches;
}

bool RecentMatchesQueue::isEmpty() const {
    return count == 0;
}

bool RecentMatchesQueue::isFull() const {
    return count == capacity;
}

void RecentMatchesQueue::addMatch(const MatchResult& match) {
    if (isFull()) {
        // Circular queue behavior - overwrite oldest match
        std::cout << "Queue full! Overwriting oldest match result." << std::endl;
        front = (front + 1) % capacity;
        count--;
    }
    
    rear = (rear + 1) % capacity;
    matches[rear] = match;
    count++;
    
    std::cout << "Match #" << match.matchId << " recorded successfully!" << std::endl;
}

MatchResult RecentMatchesQueue::removeOldestMatch() {
    if (isEmpty()) {
        std::cout << "No matches to remove - queue is empty!" << std::endl;
        return MatchResult();
    }
    
    MatchResult oldestMatch = matches[front];
    front = (front + 1) % capacity;
    count--;
    
    return oldestMatch;
}

MatchResult RecentMatchesQueue::getMostRecentMatch() const {
    if (isEmpty()) {
        std::cout << "No recent matches available!" << std::endl;
        return MatchResult();
    }
    
    return matches[rear];
}

void RecentMatchesQueue::displayRecentMatches() const {
    if (isEmpty()) {
        std::cout << "No recent matches to display!" << std::endl;
        return;
    }
    
    std::cout << "\n========== RECENT MATCHES ==========" << std::endl;
    std::cout << "Displaying " << count << " recent matches:" << std::endl;
    
    int currentIndex = front;
    for (int i = 0; i < count; i++) {
        std::cout << "\nMatch " << (i + 1) << ":" << std::endl;
        matches[currentIndex].displayMatch();
        currentIndex = (currentIndex + 1) % capacity;
    }
    std::cout << "===================================" << std::endl;
}

int RecentMatchesQueue::getCount() const {
    return count;
}

void RecentMatchesQueue::searchMatchesByPlayer(const std::string& playerName) const {
    if (isEmpty()) {
        std::cout << "No matches to search!" << std::endl;
        return;
    }
    
    std::cout << "\n=== Matches involving " << playerName << " ===" << std::endl;
    bool found = false;
    
    int currentIndex = front;
    for (int i = 0; i < count; i++) {
        if (matches[currentIndex].player1 == playerName || 
            matches[currentIndex].player2 == playerName) {
            matches[currentIndex].displayMatch();
            found = true;
        }
        currentIndex = (currentIndex + 1) % capacity;
    }
    
    if (!found) {
        std::cout << "No matches found for player: " << playerName << std::endl;
    }
}

void RecentMatchesQueue::getMatchesByGameMode(const std::string& gameMode) const {
    if (isEmpty()) {
        std::cout << "No matches to search!" << std::endl;
        return;
    }
    
    std::cout << "\n=== " << gameMode << " Matches ===" << std::endl;
    bool found = false;
    
    int currentIndex = front;
    for (int i = 0; i < count; i++) {
        if (matches[currentIndex].gameMode == gameMode) {
            matches[currentIndex].displayMatch();
            found = true;
        }
        currentIndex = (currentIndex + 1) % capacity;
    }
    
    if (!found) {
        std::cout << "No matches found for game mode: " << gameMode << std::endl;
    }
}

void RecentMatchesQueue::getMatchesByDate(const std::string& date) const {
    if (isEmpty()) {
        std::cout << "No matches to search!" << std::endl;
        return;
    }
    
    std::cout << "\n=== Matches on " << date << " ===" << std::endl;
    bool found = false;
    
    int currentIndex = front;
    for (int i = 0; i < count; i++) {
        if (matches[currentIndex].matchDate == date) {
            matches[currentIndex].displayMatch();
            found = true;
        }
        currentIndex = (currentIndex + 1) % capacity;
    }
    
    if (!found) {
        std::cout << "No matches found for date: " << date << std::endl;
    }
}

// ==================== PerformanceHistoryManager Implementation ====================

PerformanceHistoryManager::PerformanceHistoryManager(int maxPlayers, int recentMatchesSize) {
    this->maxPlayers = maxPlayers;
    currentPlayerCount = 0;
    playerDatabase = new PlayerStats[maxPlayers];
    recentMatches = new RecentMatchesQueue(recentMatchesSize);
}

PerformanceHistoryManager::~PerformanceHistoryManager() {
    delete[] playerDatabase;
    delete recentMatches;
}

int PerformanceHistoryManager::findPlayerIndex(const std::string& playerName) {
    for (int i = 0; i < currentPlayerCount; i++) {
        if (playerDatabase[i].playerName == playerName) {
            return i;
        }
    }
    return -1; // Player not found
}

void PerformanceHistoryManager::recordMatchResult(const MatchResult& match) {
    // Add match to recent matches queue
    recentMatches->addMatch(match);
    
    // Update player statistics
    // Check if players exist in database, if not add them
    int player1Index = findPlayerIndex(match.player1);
    if (player1Index == -1 && currentPlayerCount < maxPlayers) {
        playerDatabase[currentPlayerCount] = PlayerStats(match.player1);
        player1Index = currentPlayerCount;
        currentPlayerCount++;
    }
    
    int player2Index = findPlayerIndex(match.player2);
    if (player2Index == -1 && currentPlayerCount < maxPlayers) {
        playerDatabase[currentPlayerCount] = PlayerStats(match.player2);
        player2Index = currentPlayerCount;
        currentPlayerCount++;
    }
    
    // Update statistics for both players
    if (player1Index != -1) {
        bool player1Won = (match.winner == match.player1);
        playerDatabase[player1Index].updateStats(player1Won, match.player1Score);
    }
    
    if (player2Index != -1) {
        bool player2Won = (match.winner == match.player2);
        playerDatabase[player2Index].updateStats(player2Won, match.player2Score);
    }
    
    std::cout << "Player statistics updated successfully!" << std::endl;
}

PlayerStats PerformanceHistoryManager::getPlayerStats(const std::string& playerName) {
    int index = findPlayerIndex(playerName);
    if (index != -1) {
        return playerDatabase[index];
    } else {
        std::cout << "Player not found: " << playerName << std::endl;
        return PlayerStats();
    }
}

void PerformanceHistoryManager::displayAllPlayerStats() const {
    if (currentPlayerCount == 0) {
        std::cout << "No player statistics available!" << std::endl;
        return;
    }
    
    std::cout << "\n========== ALL PLAYER STATISTICS ==========" << std::endl;
    for (int i = 0; i < currentPlayerCount; i++) {
        playerDatabase[i].displayStats();
        std::cout << std::endl;
    }
    std::cout << "===========================================" << std::endl;
}

void PerformanceHistoryManager::displayRecentMatches() const {
    recentMatches->displayRecentMatches();
}

void PerformanceHistoryManager::getTopPlayersByWinRate(int topN) const {
    if (currentPlayerCount == 0) {
        std::cout << "No players to rank!" << std::endl;
        return;
    }
    
    // Create a copy of player database for sorting
    PlayerStats* sortedPlayers = new PlayerStats[currentPlayerCount];
    for (int i = 0; i < currentPlayerCount; i++) {
        sortedPlayers[i] = playerDatabase[i];
    }
    
    // Simple bubble sort by win rate (descending)
    for (int i = 0; i < currentPlayerCount - 1; i++) {
        for (int j = 0; j < currentPlayerCount - i - 1; j++) {
            if (sortedPlayers[j].winRate < sortedPlayers[j + 1].winRate) {
                PlayerStats temp = sortedPlayers[j];
                sortedPlayers[j] = sortedPlayers[j + 1];
                sortedPlayers[j + 1] = temp;
            }
        }
    }
    
    std::cout << "\n========== TOP " << topN << " PLAYERS BY WIN RATE ==========" << std::endl;
    int displayCount = (topN < currentPlayerCount) ? topN : currentPlayerCount;
    
    for (int i = 0; i < displayCount; i++) {
        std::cout << "Rank " << (i + 1) << ":" << std::endl;
        sortedPlayers[i].displayStats();
        std::cout << std::endl;
    }
    
    delete[] sortedPlayers;
}

void PerformanceHistoryManager::getTopPlayersByScore(int topN) const {
    if (currentPlayerCount == 0) {
        std::cout << "No players to rank!" << std::endl;
        return;
    }
    
    // Create a copy of player database for sorting
    PlayerStats* sortedPlayers = new PlayerStats[currentPlayerCount];
    for (int i = 0; i < currentPlayerCount; i++) {
        sortedPlayers[i] = playerDatabase[i];
    }
    
    // Simple bubble sort by average score (descending)
    for (int i = 0; i < currentPlayerCount - 1; i++) {
        for (int j = 0; j < currentPlayerCount - i - 1; j++) {
            if (sortedPlayers[j].averageScore < sortedPlayers[j + 1].averageScore) {
                PlayerStats temp = sortedPlayers[j];
                sortedPlayers[j] = sortedPlayers[j + 1];
                sortedPlayers[j + 1] = temp;
            }
        }
    }
    
    std::cout << "\n========== TOP " << topN << " PLAYERS BY AVERAGE SCORE ==========" << std::endl;
    int displayCount = (topN < currentPlayerCount) ? topN : currentPlayerCount;
    
    for (int i = 0; i < displayCount; i++) {
        std::cout << "Rank " << (i + 1) << ":" << std::endl;
        sortedPlayers[i].displayStats();
        std::cout << std::endl;
    }
    
    delete[] sortedPlayers;
}

void PerformanceHistoryManager::searchPlayerMatches(const std::string& playerName) const {
    recentMatches->searchMatchesByPlayer(playerName);
}

void PerformanceHistoryManager::generateTournamentSummary() const {
    std::cout << "\n========== TOURNAMENT SUMMARY ==========" << std::endl;
    std::cout << "Total Players: " << currentPlayerCount << std::endl;
    std::cout << "Recent Matches Stored: " << recentMatches->getCount() << std::endl;
    
    if (currentPlayerCount > 0) {
        int totalMatches = 0;
        for (int i = 0; i < currentPlayerCount; i++) {
            totalMatches += playerDatabase[i].totalMatches;
        }
        totalMatches /= 2; // Each match involves 2 players
        
        std::cout << "Total Matches Played: " << totalMatches << std::endl;
        
        // Find player with highest win rate
        int bestPlayerIndex = 0;
        for (int i = 1; i < currentPlayerCount; i++) {
            if (playerDatabase[i].winRate > playerDatabase[bestPlayerIndex].winRate) {
                bestPlayerIndex = i;
            }
        }
        
        std::cout << "Best Performing Player: " << playerDatabase[bestPlayerIndex].playerName 
                << " (Win Rate: " << std::fixed << std::setprecision(2) 
                << playerDatabase[bestPlayerIndex].winRate << "%)" << std::endl;
    }
    std::cout << "========================================" << std::endl;
}

void PerformanceHistoryManager::generatePerformanceReport() const {
    std::cout << "\n========== PERFORMANCE ANALYSIS REPORT ==========" << std::endl;
    
    if (currentPlayerCount == 0) {
        std::cout << "No data available for analysis." << std::endl;
        return;
    }
    
    // Calculate tournament statistics
    int totalGames = 0;
    int totalScores = 0;
    double totalWinRate = 0.0;
    
    for (int i = 0; i < currentPlayerCount; i++) {
        totalGames += playerDatabase[i].totalMatches;
        totalScores += playerDatabase[i].totalScore;
        totalWinRate += playerDatabase[i].winRate;
    }
    
    std::cout << "Tournament Statistics:" << std::endl;
    std::cout << "- Total Games Recorded: " << totalGames / 2 << std::endl;
    std::cout << "- Average Score per Game: " << std::fixed << std::setprecision(2) 
              << (double)totalScores / totalGames << std::endl;
    std::cout << "- Average Win Rate: " << totalWinRate / currentPlayerCount << "%" << std::endl;
    
    std::cout << "\nTop 3 Performers:" << std::endl;
    getTopPlayersByWinRate(3);
    
    std::cout << "=================================================" << std::endl;
}

void PerformanceHistoryManager::exportPlayerStatsToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not create file " << filename << std::endl;
        return;
    }
    
    file << "Player Name,Total Matches,Wins,Losses,Total Score,Win Rate,Average Score\n";
    
    for (int i = 0; i < currentPlayerCount; i++) {
        file << playerDatabase[i].playerName << ","
            << playerDatabase[i].totalMatches << ","
            << playerDatabase[i].wins << ","
            << playerDatabase[i].losses << ","
            << playerDatabase[i].totalScore << ","
            << std::fixed << std::setprecision(2) << playerDatabase[i].winRate << ","
            << playerDatabase[i].averageScore << "\n";
    }
    
    file.close();
    std::cout << "Player statistics exported to " << filename << " successfully!" << std::endl;
}

int PerformanceHistoryManager::getTotalMatchesRecorded() const {
    return recentMatches->getCount();
}

void PerformanceHistoryManager::clearAllData() {
    // Clear player database
    currentPlayerCount = 0;
    
    // Clear recent matches queue
    delete recentMatches;
    recentMatches = new RecentMatchesQueue(50);
    
    std::cout << "All tournament data cleared successfully!" << std::endl;
}

void PerformanceHistoryManager::getPlayerHeadToHead(const std::string& player1, const std::string& player2) const {
    std::cout << "\n=== Head-to-Head: " << player1 << " vs " << player2 << " ===" << std::endl;
    
    if (recentMatches->isEmpty()) {
        std::cout << "No matches to analyze!" << std::endl;
        return;
    }
    
    int player1Wins = 0;
    int player2Wins = 0;
    int totalMeetings = 0;

    std::cout << "Head-to-head analysis requires access to full match history." << std::endl;
    std::cout << "Current implementation stores only recent matches in circular queue." << std::endl;
}

void PerformanceHistoryManager::getGameModeStatistics() const {
    std::cout << "\n=== Game Mode Statistics ===" << std::endl;
    
    if (recentMatches->isEmpty()) {
        std::cout << "No matches to analyze!" << std::endl;
        return;
    }
    
    std::cout << "Game mode analysis available through recent matches:" << std::endl;
    std::cout << "Use getMatchesByGameMode() for specific game mode filtering." << std::endl;
}

void PerformanceHistoryManager::getMatchesByDateRange(const std::string& startDate, const std::string& endDate) const {
    std::cout << "\n=== Matches from " << startDate << " to " << endDate << " ===" << std::endl;
    
    if (recentMatches->isEmpty()) {
        std::cout << "No matches to search!" << std::endl;
        return;
    }
    
    std::cout << "Date range filtering available through recent matches." << std::endl;
    std::cout << "Use getMatchesByDate() for specific date filtering." << std::endl;
}

// ==================== Menu and Main Functions ====================

void displayMenu() {
    std::cout << "\n========== APUEC GAME RESULT LOGGING & PERFORMANCE HISTORY ==========\n";
    std::cout << "1.  Load Match Results from CSV\n";
    std::cout << "2.  Load Players from CSV\n";
    std::cout << "3.  Record New Match\n";
    std::cout << "4.  Display All Player Statistics\n";
    std::cout << "5.  Display Recent Matches\n";
    std::cout << "6.  Search Player Matches\n";
    std::cout << "7.  Get Top Players by Win Rate\n";
    std::cout << "8.  Get Top Players by Average Score\n";
    std::cout << "9.  Generate Tournament Summary\n";
    std::cout << "10. Generate Performance Report\n";
    std::cout << "11. Export Player Statistics to File\n";
    std::cout << "12. Clear All Data\n";
    std::cout << "0.  Exit\n";
    std::cout << "Enter your choice: ";
}

void recordNewMatch(PerformanceHistoryManager& manager) {
    int matchId, player1Score, player2Score, duration;
    std::string player1, player2, gameMode, matchDate;
    
    std::cout << "\n=== Record New Match ===\n";
    std::cout << "Enter Match ID: ";
    std::cin >> matchId;
    std::cin.ignore();
    std::cout << "Enter Player 1 Name: ";
    std::getline(std::cin, player1);
    std::cout << "Enter Player 2 Name: ";
    std::getline(std::cin, player2);
    std::cout << "Enter Player 1 Score: ";
    std::cin >> player1Score;
    std::cout << "Enter Player 2 Score: ";
    std::cin >> player2Score;
    std::cin.ignore();
    std::cout << "Enter Game Mode: ";
    std::getline(std::cin, gameMode);
    std::cout << "Enter Match Date (YYYY-MM-DD): ";
    std::getline(std::cin, matchDate);
    std::cout << "Enter Match Duration (minutes): ";
    std::cin >> duration;
    
    std::string winner = (player1Score > player2Score) ? player1 : player2;
    std::string loser = (player1Score > player2Score) ? player2 : player1;
    
    MatchResult match(matchId, player1, player2, winner, loser, player1Score, player2Score, gameMode, matchDate, duration);
    manager.recordMatchResult(match);
}

int main() {
    PerformanceHistoryManager manager(100, 50); // Max 100 players, store 50 recent matches
    int choice;
    std::string input;
    
    std::cout << "=== ASIA PACIFIC UNIVERSITY ESPORTS CHAMPIONSHIP ===\n";
    std::cout << "===        GAME RESULT LOGGING SYSTEM             ===\n";
    std::cout << "===              Task 4 - Demo                    ===\n\n";
    
    do {
        displayMenu();
        std::cin >> choice;
        std::cin.ignore(); // Clear buffer
        
        switch (choice) {
            case 1:
                std::cout << "Enter CSV filename for match results: ";
                std::getline(std::cin, input);
                manager.loadMatchResultsFromCSV(input);
                break;
            case 2:
                std::cout << "Enter CSV filename for players: ";
                std::getline(std::cin, input);
                manager.loadPlayersFromCSV(input);
                break;
            case 3:
                recordNewMatch(manager);
                break;
            case 4:
                manager.displayAllPlayerStats();
                break;
            case 5:
                manager.displayRecentMatches();
                break;
            case 6:
                std::cout << "Enter player name to search matches: ";
                std::getline(std::cin, input);
                manager.searchPlayerMatches(input);
                break;
            case 7:
                std::cout << "Enter number of top players by win rate to display: ";
                std::cin >> choice;
                manager.getTopPlayersByWinRate(choice);
                break;
            case 8:
                std::cout << "Enter number of top players by average score to display: ";
                std::cin >> choice;
                manager.getTopPlayersByScore(choice);
                break;
            case 9:
                manager.generateTournamentSummary();
                break;
            case 10:
                manager.generatePerformanceReport();
                break;
            case 11:
                std::cout << "Enter filename to export player statistics: ";
                std::getline(std::cin, input);
                manager.exportPlayerStatsToFile(input);
                break;
            case 12:
                manager.clearAllData();
                break;
            case 0:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice! Please try again.\n";
                break;
        }
        
        if (choice != 0) {
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }
        
    } while (choice != 0);
    
    return 0;
}
