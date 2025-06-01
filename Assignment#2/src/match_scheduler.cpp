#include "match_scheduler.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <ctime>
#include <algorithm>
#include <iomanip> // Required for std::setw

static const int INITIAL_CAPACITY = 64;
static const int MAX_TEAMS = 32;
static const int MAX_MATCHES = 100;

MatchScheduler::MatchScheduler() 
    : heapSize(0), heapCapacity(INITIAL_CAPACITY), playerCount(0),
      matchQueueHead(nullptr), matchQueueTail(nullptr), completedCount(0), maxPlayers(MAX_TEAMS) {
    
    playerHeap = new Player*[heapCapacity];
    allPlayers = new Player[maxPlayers];
    completedMatches = new Match[MAX_MATCHES];
    
    // Initialize tournament structure
    currentRound = "Round of 16";
    teamsRemaining = 16;
}

MatchScheduler::~MatchScheduler() {
    clearAll();
    delete[] playerHeap;
    delete[] allPlayers;
    delete[] completedMatches;
    
    // Clear match queue
    MatchNode* current = matchQueueHead;
    while (current) {
        MatchNode* temp = current->next;
        delete current->match;
        delete current;
        current = temp;
    }
}

void MatchScheduler::loadTeams(const std::string& filename) {
    std::ifstream file("data/teams.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    
    std::string line;
    // Skip header
    if (!std::getline(file, line)) return;
    
    playerCount = 0;
    while (std::getline(file, line) && playerCount < 16) { // Only load first 16 teams
        if (line.empty()) continue;
        
        std::string fields[3];
        parseLine(line, fields, 3);
        
        Player* team = &allPlayers[playerCount];
        team->playerID = fields[0];
        team->playerName = fields[1];
        team->teamID = fields[0]; // Same as playerID for teams
        team->rank = std::stoi(fields[2]);
        team->arrivalOrder = playerCount + 1;
        team->currentStage = "active";
        team->isActive = true;
        team->wins = 0;
        team->losses = 0;
        team->totalScore = 0;
        
        playerCount++;
    }
    file.close();
    std::cout << "Loaded " << playerCount << " teams for tournament.\n";
    
    // Sort teams by rank for fair bracket seeding
    sortTeamsByRank();
}

void MatchScheduler::loadPlayers(const std::string& filename) {
    // This function is now redundant since we work with teams
    std::cout << "Working with teams instead of individual players.\n";
}

void MatchScheduler::loadResults(const std::string& filename) {
    std::ifstream file("data/results.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    
    std::string line;
    // Skip header
    if (!std::getline(file, line)) return;
    
    while (std::getline(file, line) && completedCount < MAX_MATCHES) {
        if (line.empty()) continue;
        
        std::string fields[7];
        parseLine(line, fields, 7);
        
        Match* m = &completedMatches[completedCount];
        m->matchID = fields[0];
        m->teamA = fields[1];
        m->teamB = fields[2];
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
    
    // Clear existing matches
    clearMatchQueue();
    
    // Generate matches for current round
    generateCurrentRoundMatches();
}

void MatchScheduler::generateCurrentRoundMatches() {
    // Count active teams
    int activeTeams = 0;
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].isActive) {
            activeTeams++;
        }
    }
    
    if (activeTeams < 2) {
        std::cout << "Tournament complete! Champion found!\n";
        return;
    }
    
    // Determine current round name
    if (activeTeams == 16) currentRound = "Round of 16";
    else if (activeTeams == 8) currentRound = "Quarter-finals";
    else if (activeTeams == 4) currentRound = "Semi-finals";
    else if (activeTeams == 2) currentRound = "Final";
    
    std::cout << "\n=== " << currentRound << " ===\n";
    
    // Create matches by pairing active teams
    int matchCount = 0;
    for (int i = 0; i < playerCount - 1; i++) {
        if (allPlayers[i].isActive) {
            // Find next active team
            for (int j = i + 1; j < playerCount; j++) {
                if (allPlayers[j].isActive) {
                    // Create match
                    Match* newMatch = new Match();
                    newMatch->matchID = "M" + std::to_string(completedCount + matchCount + 1);
                    newMatch->teamA = allPlayers[i].teamID;
                    newMatch->teamB = allPlayers[j].teamID;
                    newMatch->scoreA = 0;
                    newMatch->scoreB = 0;
                    newMatch->status = "scheduled";
                    newMatch->round = currentRound;
                    
                    // Add to match queue
                    MatchNode* node = new MatchNode(newMatch);
                    if (!matchQueueHead) {
                        matchQueueHead = matchQueueTail = node;
                    } else {
                        matchQueueTail->next = node;
                        matchQueueTail = node;
                    }
                    
                    std::cout << "Match " << newMatch->matchID << ": " 
                              << allPlayers[i].playerName << " vs " 
                              << allPlayers[j].playerName << "\n";
                    
                    // Mark both teams as paired for this round
                    allPlayers[i].isActive = false; // Temporarily inactive until match result
                    allPlayers[j].isActive = false;
                    matchCount++;
                    break;
                }
            }
        }
        if (matchCount >= activeTeams / 2) break;
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
    std::cout << "Team A: " << getTeamName(nextMatch->teamA) << "\n";
    std::cout << "Team B: " << getTeamName(nextMatch->teamB) << "\n";
    
    delete temp;
    return nextMatch;
}

void MatchScheduler::recordMatchResult(const std::string& matchID, const std::string& winner) {
    // Get scores from user
    std::cout << "Enter score for Team A: ";
    int scoreA;
    std::cin >> scoreA;
    std::cout << "Enter score for Team B: ";
    int scoreB;
    std::cin >> scoreB;
    
    // Create new result and save to file
    saveMatchResult(matchID, winner, scoreA, scoreB);
    
    // Update team stats
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].teamID == winner && !allPlayers[i].isActive) {
            allPlayers[i].wins++;
            allPlayers[i].totalScore += (allPlayers[i].teamID == winner) ? 
                ((scoreA > scoreB) ? scoreA : scoreB) : 
                ((scoreA > scoreB) ? scoreB : scoreA);
            allPlayers[i].isActive = true; // Winner advances
            std::cout << allPlayers[i].playerName << " advances to next round!\n";
        } else if ((allPlayers[i].teamID != winner) && !allPlayers[i].isActive) {
            // Check if this team was in the match
            allPlayers[i].losses++;
            allPlayers[i].isActive = false; // Loser is eliminated
            std::cout << allPlayers[i].playerName << " is eliminated!\n";
        }
    }
}

void MatchScheduler::saveMatchResult(const std::string& matchID, const std::string& winner, int scoreA, int scoreB) {
    std::ofstream file("data/tournament_results.csv", std::ios::app);
    
    // Check if file is empty to write header
    std::ifstream checkFile("data/tournament_results.csv");
    bool isEmpty = checkFile.peek() == std::ifstream::traits_type::eof();
    checkFile.close();
    
    if (isEmpty) {
        file << "MatchID,TeamA,TeamB,ScoreA,ScoreB,Winner,Timestamp,Round\n";
    }
    
    // Get current timestamp
    time_t now = time(0);
    char* timeStr = ctime(&now);
    std::string timestamp(timeStr);
    timestamp.pop_back(); // Remove newline
    
    // Find match details
    std::string teamA, teamB;
    for (int i = 0; i < playerCount; i++) {
        if (!allPlayers[i].isActive || allPlayers[i].teamID == winner) {
            if (teamA.empty()) teamA = allPlayers[i].teamID;
            else if (teamB.empty() && allPlayers[i].teamID != teamA) {
                teamB = allPlayers[i].teamID;
                break;
            }
        }
    }
    
    file << matchID << "," << teamA << "," << teamB << "," 
         << scoreA << "," << scoreB << "," << winner << "," 
         << timestamp << "," << currentRound << "\n";
    
    file.close();
    std::cout << "Match result saved to tournament_results.csv\n";
}

void MatchScheduler::displayTournamentBracket() {
    std::cout << "\n=== TOURNAMENT BRACKET VISUALIZATION ===\n";
    
    // Count teams in each stage
    int active = 0, eliminated = 0;
    
    std::cout << "\n--- ACTIVE TEAMS ---\n";
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].isActive) {
            std::cout << "🏆 " << allPlayers[i].playerName 
                      << " (Rank: " << allPlayers[i].rank
                      << ") W:" << allPlayers[i].wins 
                      << " Score:" << allPlayers[i].totalScore << "\n";
            active++;
        }
    }
    
    std::cout << "\n--- ELIMINATED TEAMS ---\n";
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
    std::cout << "Teams Remaining: " << active << "\n";
    
    // Show tournament progression
    std::cout << "\n--- TOURNAMENT PROGRESSION ---\n";
    std::cout << "Round of 16 → Quarter-finals → Semi-finals → Final → Champion\n";
    std::cout << "     16    →        8        →      4      →   2   →    1\n";
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
        std::cout << m->matchID << ": " << getTeamName(m->teamA) 
                  << " vs " << getTeamName(m->teamB) 
                  << " [" << m->status << "]\n";
        current = current->next;
    }
}

void MatchScheduler::displayPlayerStats() {
    std::cout << "\n=== TEAM STATISTICS ===\n";
    
    // Sort teams by performance (wins, then total score)
    Player* sortedTeams[32];
    for (int i = 0; i < playerCount; i++) {
        sortedTeams[i] = &allPlayers[i];
    }
    
    // Simple bubble sort by wins (descending)
    for (int i = 0; i < playerCount - 1; i++) {
        for (int j = 0; j < playerCount - i - 1; j++) {
            if (sortedTeams[j]->wins < sortedTeams[j + 1]->wins || 
                (sortedTeams[j]->wins == sortedTeams[j + 1]->wins && 
                 sortedTeams[j]->totalScore < sortedTeams[j + 1]->totalScore)) {
                Player* temp = sortedTeams[j];
                sortedTeams[j] = sortedTeams[j + 1];
                sortedTeams[j + 1] = temp;
            }
        }
    }
    
    std::cout << "Rank | Team Name               | W | L | Total Score | Status\n";
    std::cout << "-----|-------------------------|---|---|-------------|--------\n";
    
    for (int i = 0; i < playerCount; i++) {
        Player* team = sortedTeams[i];
        std::cout << std::setw(4) << (i + 1) << " | " 
                  << std::setw(23) << team->playerName << " | "
                  << team->wins << " | " << team->losses << " | "
                  << std::setw(11) << team->totalScore << " | "
                  << (team->isActive ? "Active" : "Eliminated") << "\n";
    }
}

std::string MatchScheduler::getTeamName(const std::string& teamID) {
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].teamID == teamID) {
            return allPlayers[i].playerName;
        }
    }
    return teamID;
}

void MatchScheduler::sortTeamsByRank() {
    // Sort teams by rank (ascending - better rank = lower number)
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
    // Better rank = lower number, so lower rank gets higher priority
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

void MatchScheduler::updatePlayerStats() {
    std::cout << "Team stats updated based on match results.\n";
}