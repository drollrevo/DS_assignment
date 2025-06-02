#include "task4.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cstring>

char** splitCSVLine(const std::string& line, int& size) {
    size = 1;
    for(size_t i = 0; i < line.length(); i++) {
        if(line[i] == ',') size++;
    }
    
    char** result = new char*[size];
    char* buffer = new char[line.length() + 1];
    strcpy(buffer, line.c_str());
    
    int index = 0;
    char* token = strtok(buffer, ",");
    while(token != nullptr && index < size) {
        while(*token == ' ' || *token == '\t' || *token == '\r' || *token == '\n') 
            token++;
        int len = strlen(token);
        while(len > 0 && (token[len-1] == ' ' || token[len-1] == '\t' || 
              token[len-1] == '\r' || token[len-1] == '\n')) {
            token[len-1] = '\0';
            len--;
        }
        result[index] = new char[len + 1];
        strcpy(result[index], token);
        index++;
        token = strtok(nullptr, ",");
    }
    
    delete[] buffer;
    return result;
}

void cleanupCSVArray(char** array, int size) {
    for(int i = 0; i < size; i++) {
        delete[] array[i];
    }
    delete[] array;
}

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
        
        if (fieldCount >= 8) {
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

bool isValidDate(const std::string& date) {
    return !date.empty() && date.length() >= 8;
}

std::string getCurrentDate() {
    return "2024-12-01";
}

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
    std::cout << "=== Match #" << matchId << " ===\n";
    std::cout << "Players: " << player1 << " vs " << player2 << "\n";
    std::cout << "Score: " << player1Score << " - " << player2Score << "\n";
    std::cout << "Winner: " << winner << "\n";
    std::cout << "Game Mode: " << gameMode << "\n";
    std::cout << "Date: " << matchDate << "\n";
    std::cout << "Duration: " << durationMinutes << " minutes\n";
    std::cout << "================================\n";
}

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
    std::cout << "=== Player Statistics ===\n";
    std::cout << "Player: " << playerName << "\n";
    std::cout << "Total Matches: " << totalMatches << "\n";
    std::cout << "Wins: " << wins << "\n";
    std::cout << "Losses: " << losses << "\n";
    std::cout << "Total Score: " << totalScore << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Win Rate: " << winRate << "%\n";
    std::cout << "Average Score: " << averageScore << "\n";
    std::cout << "=========================\n";
}

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
        std::cout << "Queue full! Overwriting oldest match result.\n";
        front = (front + 1) % capacity;
        count--;
    }
    
    rear = (rear + 1) % capacity;
    matches[rear] = match;
    count++;
    
    std::cout << "Match #" << match.matchId << " recorded successfully!\n";
}

MatchResult RecentMatchesQueue::removeOldestMatch() {
    if (isEmpty()) {
        std::cout << "No matches to remove - queue is empty!\n";
        return MatchResult();
    }
    
    MatchResult oldestMatch = matches[front];
    front = (front + 1) % capacity;
    count--;
    
    return oldestMatch;
}

MatchResult RecentMatchesQueue::getMostRecentMatch() const {
    if (isEmpty()) {
        std::cout << "No recent matches available!\n";
        return MatchResult();
    }
    
    return matches[rear];
}

void RecentMatchesQueue::displayRecentMatches() const {
    if (isEmpty()) {
        std::cout << "No recent matches to display!\n";
        return;
    }
    
    std::cout << "\n========== RECENT MATCHES ==========\n";
    std::cout << "Displaying " << count << " recent matches:\n";
    
    int currentIndex = front;
    for (int i = 0; i < count; i++) {
        std::cout << "\nMatch " << (i + 1) << ":\n";
        matches[currentIndex].displayMatch();
        currentIndex = (currentIndex + 1) % capacity;
    }
    std::cout << "===================================\n";
}

int RecentMatchesQueue::getCount() const {
    return count;
}

void RecentMatchesQueue::searchMatchesByPlayer(const std::string& playerName) const {
    if (isEmpty()) {
        std::cout << "No matches to search!\n";
        return;
    }
    
    std::cout << "\n=== Matches involving " << playerName << " ===\n";
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
        std::cout << "No matches found for player: " << playerName << "\n";
    }
}

void RecentMatchesQueue::getMatchesByGameMode(const std::string& gameMode) const {
    if (isEmpty()) {
        std::cout << "No matches to search!\n";
        return;
    }
    
    std::cout << "\n=== " << gameMode << " Matches ===\n";
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
        std::cout << "No matches found for game mode: " << gameMode << "\n";
    }
}

void RecentMatchesQueue::getMatchesByDate(const std::string& date) const {
    if (isEmpty()) {
        std::cout << "No matches to search!\n";
        return;
    }
    
    std::cout << "\n=== Matches on " << date << " ===\n";
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
        std::cout << "No matches found for date: " << date << "\n";
    }
}

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
    return -1;
}

void PerformanceHistoryManager::recordMatchResult(const MatchResult& match) {
    recentMatches->addMatch(match);
    
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
    
    if (player1Index != -1) {
        bool player1Won = (match.winner == match.player1);
        playerDatabase[player1Index].updateStats(player1Won, match.player1Score);
    }
    
    if (player2Index != -1) {
        bool player2Won = (match.winner == match.player2);
        playerDatabase[player2Index].updateStats(player2Won, match.player2Score);
    }
    
    std::cout << "Player statistics updated successfully!\n";
}

PlayerStats PerformanceHistoryManager::getPlayerStats(const std::string& playerName) {
    int index = findPlayerIndex(playerName);
    if (index != -1) {
        return playerDatabase[index];
    } else {
        std::cout << "Player not found: " << playerName << "\n";
        return PlayerStats();
    }
}

void PerformanceHistoryManager::displayAllPlayerStats() const {
    if (currentPlayerCount == 0) {
        std::cout << "No player statistics available!\n";
        return;
    }
    
    std::cout << "\n========== ALL PLAYER STATISTICS ==========\n";
    for (int i = 0; i < currentPlayerCount; i++) {
        playerDatabase[i].displayStats();
        std::cout << "\n";
    }
    std::cout << "===========================================\n";
}

void PerformanceHistoryManager::displayRecentMatches() const {
    recentMatches->displayRecentMatches();
}

void PerformanceHistoryManager::getTopPlayersByWinRate(int topN) const {
    if (currentPlayerCount == 0) {
        std::cout << "No players to rank!\n";
        return;
    }
    
    PlayerStats* sortedPlayers = new PlayerStats[currentPlayerCount];
    for (int i = 0; i < currentPlayerCount; i++) {
        sortedPlayers[i] = playerDatabase[i];
    }
    
    for (int i = 0; i < currentPlayerCount - 1; i++) {
        for (int j = 0; j < currentPlayerCount - i - 1; j++) {
            if (sortedPlayers[j].winRate < sortedPlayers[j + 1].winRate) {
                PlayerStats temp = sortedPlayers[j];
                sortedPlayers[j] = sortedPlayers[j + 1];
                sortedPlayers[j + 1] = temp;
            }
        }
    }
    
    std::cout << "\n========== TOP " << topN << " PLAYERS BY WIN RATE ==========\n";
    int displayCount = (topN < currentPlayerCount) ? topN : currentPlayerCount;
    
    for (int i = 0; i < displayCount; i++) {
        std::cout << "Rank " << (i + 1) << ":\n";
        sortedPlayers[i].displayStats();
        std::cout << "\n";
    }
    
    delete[] sortedPlayers;
}

void PerformanceHistoryManager::getTopPlayersByScore(int topN) const {
    if (currentPlayerCount == 0) {
        std::cout << "No players to rank!\n";
        return;
    }
    
    PlayerStats* sortedPlayers = new PlayerStats[currentPlayerCount];
    for (int i = 0; i < currentPlayerCount; i++) {
        sortedPlayers[i] = playerDatabase[i];
    }
    
    for (int i = 0; i < currentPlayerCount - 1; i++) {
        for (int j = 0; j < currentPlayerCount - i - 1; j++) {
            if (sortedPlayers[j].averageScore < sortedPlayers[j + 1].averageScore) {
                PlayerStats temp = sortedPlayers[j];
                sortedPlayers[j] = sortedPlayers[j + 1];
                sortedPlayers[j + 1] = temp;
            }
        }
    }
    
    std::cout << "\n========== TOP " << topN << " PLAYERS BY AVERAGE SCORE ==========\n";
    int displayCount = (topN < currentPlayerCount) ? topN : currentPlayerCount;
    
    for (int i = 0; i < displayCount; i++) {
        std::cout << "Rank " << (i + 1) << ":\n";
        sortedPlayers[i].displayStats();
        std::cout << "\n";
    }
    
    delete[] sortedPlayers;
}

void PerformanceHistoryManager::searchPlayerMatches(const std::string& playerName) const {
    recentMatches->searchMatchesByPlayer(playerName);
}

void PerformanceHistoryManager::generateTournamentSummary() const {
    std::cout << "\n========== TOURNAMENT SUMMARY ==========\n";
    std::cout << "Total Players: " << currentPlayerCount << "\n";
    std::cout << "Recent Matches Stored: " << recentMatches->getCount() << "\n";
    
    if (currentPlayerCount > 0) {
        int totalMatches = 0;
        for (int i = 0; i < currentPlayerCount; i++) {
            totalMatches += playerDatabase[i].totalMatches;
        }
        totalMatches /= 2;
        
        std::cout << "Total Matches Played: " << totalMatches << "\n";
        
        int bestPlayerIndex = 0;
        for (int i = 1; i < currentPlayerCount; i++) {
            if (playerDatabase[i].winRate > playerDatabase[bestPlayerIndex].winRate) {
                bestPlayerIndex = i;
            }
        }
        
        std::cout << "Best Performing Player: " << playerDatabase[bestPlayerIndex].playerName 
                << " (Win Rate: " << std::fixed << std::setprecision(2) 
                << playerDatabase[bestPlayerIndex].winRate << "%)\n";
    }
    std::cout << "========================================" << std::endl;
}

void PerformanceHistoryManager::generatePerformanceReport() const {
    std::cout << "\n========== PERFORMANCE ANALYSIS REPORT ==========\n";
    
    if (currentPlayerCount == 0) {
        std::cout << "No data available for analysis.\n";
        return;
    }
    
    int totalGames = 0;
    int totalScores = 0;
    double totalWinRate = 0.0;
    
    for (int i = 0; i < currentPlayerCount; i++) {
        totalGames += playerDatabase[i].totalMatches;
        totalScores += playerDatabase[i].totalScore;
        totalWinRate += playerDatabase[i].winRate;
    }
    
    std::cout << "Tournament Statistics:\n";
    std::cout << "- Total Games Recorded: " << totalGames / 2 << "\n";
    std::cout << "- Average Score per Game: " << std::fixed << std::setprecision(2) 
              << (double)totalScores / totalGames << "\n";
    std::cout << "- Average Win Rate: " << totalWinRate / currentPlayerCount << "%\n";
    
    std::cout << "\nTop 3 Performers:\n";
    getTopPlayersByWinRate(3);
    
    std::cout << "=================================================\n";
}

void PerformanceHistoryManager::exportPlayerStatsToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Could not create file " << filename << "\n";
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
    std::cout << "Player statistics exported to " << filename << " successfully!\n";
}

int PerformanceHistoryManager::getTotalMatchesRecorded() const {
    return recentMatches->getCount();
}

void PerformanceHistoryManager::clearAllData() {
    currentPlayerCount = 0;
    delete recentMatches;
    recentMatches = new RecentMatchesQueue(50);
    std::cout << "All tournament data cleared successfully!\n";
}

void PerformanceHistoryManager::getPlayerHeadToHead(const std::string& player1, const std::string& player2) const {
    std::cout << "\n=== Head-to-Head: " << player1 << " vs " << player2 << " ===\n";
    
    if (recentMatches->isEmpty()) {
        std::cout << "No matches to analyze!\n";
        return;
    }
    
    std::cout << "Head-to-head analysis requires access to full match history.\n";
    std::cout << "Current implementation stores only recent matches in circular queue.\n";
}

void PerformanceHistoryManager::getGameModeStatistics() const {
    std::cout << "\n=== Game Mode Statistics ===\n";
    
    if (recentMatches->isEmpty()) {
        std::cout << "No matches to analyze!\n";
        return;
    }
    
    std::cout << "Game mode analysis available through recent matches:\n";
    std::cout << "Use getMatchesByGameMode() for specific game mode filtering.\n";
}

void PerformanceHistoryManager::getMatchesByDateRange(const std::string& startDate, const std::string& endDate) const {
    std::cout << "\n=== Matches from " << startDate << " to " << endDate << " ===\n";
    
    if (recentMatches->isEmpty()) {
        std::cout << "No matches to search!\n";
        return;
    }
    
    std::cout << "Date range filtering available through recent matches.\n";
    std::cout << "Use getMatchesByDate() for specific date filtering.\n";
}