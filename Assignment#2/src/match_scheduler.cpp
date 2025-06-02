#include "match_scheduler.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <iomanip>

static const int INITIAL_CAPACITY = 64;
static const int MAX_PLAYERS = 128;
static const int MAX_MATCHES = 100;

MatchScheduler::MatchScheduler(PerformanceHistoryManager* perfManager) 
    : perfManager(perfManager), heapSize(0), heapCapacity(INITIAL_CAPACITY), playerCount(0),
      matchQueueHead(nullptr), matchQueueTail(nullptr), completedCount(0), maxPlayers(MAX_PLAYERS) {
    
    playerHeap = new Player*[heapCapacity];
    allPlayers = new Player[maxPlayers];
    completedMatches = new Match[MAX_MATCHES];
    
    currentRound = "Round of 128";
}

MatchScheduler::~MatchScheduler() {
    clearAll();
    delete[] playerHeap;
    delete[] allPlayers;
    delete[] completedMatches;
    
    MatchNode* current = matchQueueHead;
    while (current) {
        MatchNode* temp = current->next;
        delete current->match;
        delete current;
        current = temp;
    }
}

void MatchScheduler::loadPlayers(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    
    std::string line;
    if (!std::getline(file, line)) return;
    
    playerCount = 0;
    while (std::getline(file, line) && playerCount < maxPlayers) {
        if (line.empty()) continue;
        
        std::string fields[3];
        parseLine(line, fields, 3);
        
        allPlayers[playerCount].playerID = fields[0];
        allPlayers[playerCount].playerName = fields[1];
        allPlayers[playerCount].teamID = fields[2];
        allPlayers[playerCount].rank = playerCount + 1;
        allPlayers[playerCount].arrivalOrder = playerCount + 1;
        allPlayers[playerCount].currentStage = "active";
        allPlayers[playerCount].isActive = true;
        allPlayers[playerCount].wins = 0;
        allPlayers[playerCount].losses = 0;
        allPlayers[playerCount].totalScore = 0;
        
        playerCount++;
    }
    file.close();
    std::cout << "Loaded " << playerCount << " players for tournament.\n";
    
    sortPlayersByRank();
}

void MatchScheduler::sortPlayersByRank() {
    for (int i = 0; i < playerCount - 1; i++) {
        for (int j = 0; j < playerCount - i - 1; j++) {
            if (allPlayers[j].rank > allPlayers[j + 1].rank) {
                Player temp = allPlayers[j];
                allPlayers[j] = allPlayers[j + 1];
                allPlayers[j + 1] = temp;
            }
        }
    }
}

void MatchScheduler::loadResults(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    
    std::string line;
    if (!std::getline(file, line)) return;
    
    while (std::getline(file, line) && completedCount < MAX_MATCHES) {
        if (line.empty()) continue;
        
        std::string fields[7];
        parseLine(line, fields, 7);
        
        Match* m = &completedMatches[completedCount];
        m->matchID = fields[0];
        m->playerA = fields[1];
        m->playerB = fields[2];
        m->scoreA = std::stoi(fields[3]);
        m->scoreB = std::stoi(fields[4]);
        m->winner = fields[5];
        m->timestamp = fields[6];
        m->status = "completed";
        
        completedCount++;
    }
    file.close();
    std::cout << "Loaded " << completedCount << " completed matches.\n";
}

void MatchScheduler::generateMatchPairings(const std::string& stage) {
    std::cout << "\n=== Generating Tournament Bracket ===\n";
    clearMatchQueue();
    generateCurrentRoundMatches();
}

void MatchScheduler::generateCurrentRoundMatches() {
    int activePlayers = 0;
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].isActive) {
            activePlayers++;
        }
    }
    
    if (activePlayers < 2) {
        std::cout << "Tournament complete! Champion found!\n";
        return;
    }
    
    if (activePlayers == 128) currentRound = "Round of 128";
    else if (activePlayers == 64) currentRound = "Round of 64";
    else if (activePlayers == 32) currentRound = "Round of 32";
    else if (activePlayers == 16) currentRound = "Round of 16";
    else if (activePlayers == 8) currentRound = "Quarter-finals";
    else if (activePlayers == 4) currentRound = "Semi-finals";
    else if (activePlayers == 2) currentRound = "Final";
    
    std::cout << "\n=== " << currentRound << " ===\n";
    
    int matchCount = 0;
    int left = 0;
    int right = playerCount - 1;
    while (left < right) {
        while (!allPlayers[left].isActive && left < right) left++;
        while (!allPlayers[right].isActive && left < right) right--;
        if (left < right) {
            Match* newMatch = new Match();
            newMatch->matchID = "M" + std::to_string(completedCount + matchCount + 1);
            newMatch->playerA = allPlayers[left].playerID;
            newMatch->playerB = allPlayers[right].playerID;
            newMatch->scoreA = 0;
            newMatch->scoreB = 0;
            newMatch->status = "scheduled";
            newMatch->round = currentRound;
            
            MatchNode* node = new MatchNode(newMatch);
            if (!matchQueueHead) {
                matchQueueHead = matchQueueTail = node;
            } else {
                matchQueueTail->next = node;
                matchQueueTail = node;
            }
            
            std::cout << "Match " << newMatch->matchID << ": " 
                      << allPlayers[left].playerName << " vs " 
                      << allPlayers[right].playerName << "\n";
            
            allPlayers[left].isActive = false;
            allPlayers[right].isActive = false;
            matchCount++;
            left++;
            right--;
        }
    }
}

Match* MatchScheduler::getNextMatch() {
    if (!matchQueueHead) {
        std::cout << "No matches scheduled!\n";
        return nullptr;
    }
    
    Match* nextMatch = matchQueueHead->match;
    MatchNode* temp = matchQueueHead;
    matchQueueHead = matchQueueHead->next;
    if (!matchQueueHead) matchQueueTail = nullptr;
    
    std::cout << "\n=== Next Match (" << nextMatch->round << ") ===\n";
    std::cout << "Match ID: " << nextMatch->matchID << "\n";
    std::cout << "Player A: " << getPlayerName(nextMatch->playerA) << "\n";
    std::cout << "Player B: " << getPlayerName(nextMatch->playerB) << "\n";
    
    delete temp;
    return nextMatch;
}

void MatchScheduler::recordMatchResult(const std::string& matchID, const std::string& winner) {
    Match* match = nullptr;
    MatchNode* current = matchQueueHead;
    while (current) {
        if (current->match->matchID == matchID) {
            match = current->match;
            break;
        }
        current = current->next;
    }
    if (!match) {
        for (int i = 0; i < completedCount; i++) {
            if (completedMatches[i].matchID == matchID) {
                match = &completedMatches[i];
                break;
            }
        }
    }
    if (!match) {
        std::cout << "Match not found!\n";
        return;
    }
    
    std::cout << "Enter score for " << match->playerA << ": ";
    int scoreA;
    std::cin >> scoreA;
    std::cout << "Enter score for " << match->playerB << ": ";
    int scoreB;
    std::cin >> scoreB;
    
    if (winner.empty()) {
        if (scoreA > scoreB) {
            match->winner = match->playerA;
        } else if (scoreB > scoreA) {
            match->winner = match->playerB;
        } else {
            std::cout << "Tie not handled!\n";
            return;
        }
    } else {
        match->winner = winner;
    }
    
    saveMatchResult(matchID, match->winner, scoreA, scoreB);
    
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].playerID == match->playerA) {
            if (allPlayers[i].playerID == match->winner) {
                allPlayers[i].wins++;
                allPlayers[i].totalScore += scoreA;
                allPlayers[i].isActive = true;
            } else {
                allPlayers[i].losses++;
                allPlayers[i].isActive = false;
            }
        } else if (allPlayers[i].playerID == match->playerB) {
            if (allPlayers[i].playerID == match->winner) {
                allPlayers[i].wins++;
                allPlayers[i].totalScore += scoreB;
                allPlayers[i].isActive = true;
            } else {
                allPlayers[i].losses++;
                allPlayers[i].isActive = false;
            }
        }
    }
    
    MatchResult matchResult;
    matchResult.matchId = std::stoi(matchID.substr(1));
    matchResult.player1 = match->playerA;
    matchResult.player2 = match->playerB;
    matchResult.winner = match->winner;
    matchResult.loser = (match->winner == match->playerA) ? match->playerB : match->playerA;
    matchResult.player1Score = scoreA;
    matchResult.player2Score = scoreB;
    matchResult.gameMode = "1v1";
    matchResult.matchDate = "2025-06-15";
    matchResult.durationMinutes = 30;

    if (perfManager) {
        perfManager->recordMatchResult(matchResult);
    }
    
    std::cout << "Match result recorded.\n";
}

void MatchScheduler::saveMatchResult(const std::string& matchID, const std::string& winner, int scoreA, int scoreB) {
    std::ofstream file("data/tournament_results.csv", std::ios::app);
    
    std::ifstream checkFile("data/tournament_results.csv");
    bool isEmpty = checkFile.peek() == std::ifstream::traits_type::eof();
    checkFile.close();
    
    if (isEmpty) {
        file << "MatchID,PlayerA,PlayerB,ScoreA,ScoreB,Winner,Timestamp,Round\n";
    }
    
    time_t now = time(0);
    char* timeStr = ctime(&now);
    std::string timestamp(timeStr);
    timestamp.pop_back();
    
    // Find the match to get playerA and playerB
    Match* match = nullptr;
    MatchNode* current = matchQueueHead;
    while (current) {
        if (current->match->matchID == matchID) {
            match = current->match;
            break;
        }
        current = current->next;
    }
    if (!match) {
        for (int i = 0; i < completedCount; i++) {
            if (completedMatches[i].matchID == matchID) {
                match = &completedMatches[i];
                break;
            }
        }
    }
    
    if (match) {
        file << matchID << "," << match->playerA << "," << match->playerB << "," 
             << scoreA << "," << scoreB << "," << winner << "," 
             << timestamp << "," << currentRound << "\n";
    }
    
    file.close();
    std::cout << "Match result saved to tournament_results.csv\n";
}

void MatchScheduler::displayTournamentBracket() {
    std::cout << "\n=== TOURNAMENT BRACKET VISUALIZATION ===\n";
    
    int active = 0, eliminated = 0;
    
    std::cout << "\n--- ACTIVE PLAYERS ---\n";
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].isActive) {
            std::cout << "🏆 " << allPlayers[i].playerName 
                      << " (Rank: " << allPlayers[i].rank
                      << ") W:" << allPlayers[i].wins 
                      << " Score:" << allPlayers[i].totalScore << "\n";
            active++;
        }
    }
    
    std::cout << "\n--- ELIMINATED PLAYERS ---\n";
    for (int i = 0; i < playerCount; i++) {
        if (!allPlayers[i].isActive && allPlayers[i].losses > 0) {
            std::cout << "❌ " << allPlayers[i].playerName 
                      << " (Rank: " << allPlayers[i].rank
                      << ") W:" << allPlayers[i].wins 
                      << " L:" << allPlayers[i].losses << "\n";
            eliminated++;
        }
    }
    
    std::cout << "\nCurrent Round: " << currentRound << "\n";
    std::cout << "Players Remaining: " << active << "\n";
    
    std::cout << "\n--- TOURNAMENT PROGRESSION ---\n";
    std::cout << "Round of 128 → Round of 64 → Round of 32 → Round of 16 → Quarter-finals → Semi-finals → Final → Champion\n";
    std::cout << "     128    →     64     →     32     →     16     →      8      →      4      →   2   →    1\n";
}

void MatchScheduler::displayMatchSchedule() {
    std::cout << "\n=== MATCH SCHEDULE (" << currentRound << ") ===\n";
    
    MatchNode* current = matchQueueHead;
    if (!current) {
        std::cout << "No matches scheduled for current round.\n";
        return;
    }
    
    while (current) {
        Match* m = current->match;
        std::cout << m->matchID << ": " << getPlayerName(m->playerA) 
                  << " vs " << getPlayerName(m->playerB) 
                  << " [" << m->status << "]\n";
        current = current->next;
    }
}

void MatchScheduler::displayPlayerStats() {
    std::cout << "\n=== PLAYER STATISTICS ===\n";
    
    Player* sortedPlayers[128];
    for (int i = 0; i < playerCount; i++) {
        sortedPlayers[i] = &allPlayers[i];
    }
    
    for (int i = 0; i < playerCount - 1; i++) {
        for (int j = 0; j < playerCount - i - 1; j++) {
            if (sortedPlayers[j]->wins < sortedPlayers[j + 1]->wins || 
                (sortedPlayers[j]->wins == sortedPlayers[j + 1]->wins && 
                 sortedPlayers[j]->totalScore < sortedPlayers[j + 1]->totalScore)) {
                Player* temp = sortedPlayers[j];
                sortedPlayers[j] = sortedPlayers[j + 1];
                sortedPlayers[j + 1] = temp;
            }
        }
    }
    
    std::cout << "Rank | Player Name             | W | L | Total Score | Status\n";
    std::cout << "-----|-------------------------|---|---|-------------|--------\n";
    
    for (int i = 0; i < playerCount; i++) {
        Player* player = sortedPlayers[i];
        std::cout << std::setw(4) << (i + 1) << " | " 
                  << std::setw(23) << player->playerName << " | "
                  << player->wins << " | " << player->losses << " | "
                  << std::setw(11) << player->totalScore << " | "
                  << (player->isActive ? "Active" : "Eliminated") << "\n";
    }
}

std::string MatchScheduler::getPlayerName(const std::string& playerID) {
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].playerID == playerID) {
            return allPlayers[i].playerName;
        }
    }
    return playerID;
}

void MatchScheduler::clearMatchQueue() {
    MatchNode* current = matchQueueHead;
    while (current) {
        MatchNode* temp = current->next;
        delete current->match;
        delete current;
        current = temp;
    }
    matchQueueHead = matchQueueTail = nullptr;
}

bool MatchScheduler::isEmpty() const {
    return matchQueueHead == nullptr;
}

int MatchScheduler::size() const {
    int count = 0;
    MatchNode* current = matchQueueHead;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

void MatchScheduler::clearAll() {
    clearMatchQueue();
    heapSize = 0;
}

void MatchScheduler::siftUp(int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (comparePlayer(playerHeap[idx], playerHeap[parent]) < 0) {
            Player* temp = playerHeap[idx];
            playerHeap[idx] = playerHeap[parent];
            playerHeap[parent] = temp;
            idx = parent;
        } else {
            break;
        }
    }
}

void MatchScheduler::siftDown(int idx) {
    while (true) {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = idx;
        
        if (left < heapSize && comparePlayer(playerHeap[left], playerHeap[smallest]) < 0) {
            smallest = left;
        }
        if (right < heapSize && comparePlayer(playerHeap[right], playerHeap[smallest]) < 0) {
            smallest = right;
        }
        
        if (smallest != idx) {
            Player* temp = playerHeap[idx];
            playerHeap[idx] = playerHeap[smallest];
            playerHeap[smallest] = temp;
            idx = smallest;
        } else {
            break;
        }
    }
}

void MatchScheduler::resizeHeap() {
    int newCapacity = heapCapacity * 2;
    Player** newHeap = new Player*[newCapacity];
    for (int i = 0; i < heapSize; i++) {
        newHeap[i] = playerHeap[i];
    }
    delete[] playerHeap;
    playerHeap = newHeap;
    heapCapacity = newCapacity;
}

int MatchScheduler::comparePlayer(Player* a, Player* b) {
    if (a->rank < b->rank) return -1;
    if (a->rank > b->rank) return 1;
    if (a->arrivalOrder < b->arrivalOrder) return -1;
    if (a->arrivalOrder > b->arrivalOrder) return 1;
    return 0;
}

void MatchScheduler::parseLine(const std::string& line, std::string* fields, int expectedFields) {
    std::stringstream ss(line);
    std::string item;
    int idx = 0;
    while (std::getline(ss, item, ',') && idx < expectedFields) {
        fields[idx++] = item;
    }
}