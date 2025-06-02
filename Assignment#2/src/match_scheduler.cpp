#include "match_scheduler.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>

static const int INITIAL_CAPACITY = 64;
static const int MAX_TEAMS = 32;
static const int MAX_MATCHES = 100;

MatchScheduler::MatchScheduler(PerformanceHistoryManager* perfManager)
    : perfManager(perfManager), heapSize(0), heapCapacity(INITIAL_CAPACITY), teamCount(0),
      matchQueueHead(nullptr), matchQueueTail(nullptr), completedCount(0), maxTeams(MAX_TEAMS) {
    
    playerHeap = new Player*[heapCapacity];
    allTeams = new Player[maxTeams];
    completedMatches = new Match[MAX_MATCHES];
    
    currentRound = "Round of 16";
    teamsRemaining = 16;
}

MatchScheduler::~MatchScheduler() {
    clearAll();
    delete[] playerHeap;
    delete[] allTeams;
    delete[] completedMatches;
    
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
    if (!std::getline(file, line)) return; // Skip header
    
    teamCount = 0;
    while (std::getline(file, line) && teamCount < 16) {
        if (line.empty()) continue;
        
        std::string fields[3];
        parseLine(line, fields, 3);
        
        Player* team = &allTeams[teamCount];
        team->playerID = fields[0]; // Team ID
        team->playerName = fields[1]; // Team Name
        team->teamID = fields[0];
        team->rank = std::stoi(fields[2]);
        team->arrivalOrder = teamCount + 1;
        team->currentStage = "active";
        team->isActive = true;
        team->wins = 0;
        team->losses = 0;
        team->totalScore = 0;
        
        teamCount++;
    }
    file.close();
    std::cout << "Loaded " << teamCount << " teams for tournament.\n";
    
    sortTeamsByRank();
}

void MatchScheduler::sortTeamsByRank() {
    for (int i = 0; i < teamCount - 1; i++) {
        for (int j = 0; j < teamCount - i - 1; j++) {
            if (allTeams[j].rank > allTeams[j + 1].rank) {
                Player temp = allTeams[j];
                allTeams[j] = allTeams[j + 1];
                allTeams[j + 1] = temp;
            }
        }
    }
}

void MatchScheduler::loadResults(const std::string& filename) {
    std::ifstream file("data/results.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    
    std::string line;
    if (!std::getline(file, line)) return; // Skip header
    
    while (std::getline(file, line) && completedCount < MAX_MATCHES) {
        if (line.empty()) continue;
        
        std::string fields[8];
        parseLine(line, fields, 8);
        
        Match* m = &completedMatches[completedCount];
        m->matchID = fields[0];
        m->teamA = fields[1];
        m->teamB = fields[2];
        m->scoreA = std::stoi(fields[3]);
        m->scoreB = std::stoi(fields[4]);
        m->winner = fields[5];
        m->timestamp = fields[6];
        m->round = fields[7];
        m->status = "completed";
        
        completedCount++;
    }
    file.close();
    std::cout << "Loaded " << completedCount << " completed matches.\n";
}

void MatchScheduler::generateMatchPairings() {
    std::cout << "\n=== Generating Tournament Bracket ===\n";
    
    clearMatchQueue();
    
    int activeTeamsCount = 0;
    for (int i = 0; i < teamCount; i++) {
        if (allTeams[i].isActive) {
            activeTeamsCount++;
        }
    }
    
    if (activeTeamsCount < 2) {
        std::cout << "Tournament complete! Champion found!\n";
        return;
    }
    
    if (activeTeamsCount == 16) currentRound = "Round of 16";
    else if (activeTeamsCount == 8) currentRound = "Quarter-finals";
    else if (activeTeamsCount == 4) currentRound = "Semi-finals";
    else if (activeTeamsCount == 2) currentRound = "Final";
    
    std::cout << "\n=== " << currentRound << " ===\n";
    
    // Allocate array for active teams
    Player** activeTeams = new Player*[activeTeamsCount];
    int activeIndex = 0;
    for (int i = 0; i < teamCount; i++) {
        if (allTeams[i].isActive) {
            activeTeams[activeIndex++] = &allTeams[i];
        }
    }
    
    // Sort by rank
    for (int i = 0; i < activeTeamsCount - 1; i++) {
        for (int j = 0; j < activeTeamsCount - i - 1; j++) {
            if (activeTeams[j]->rank > activeTeams[j + 1]->rank) {
                Player* temp = activeTeams[j];
                activeTeams[j] = activeTeams[j + 1];
                activeTeams[j + 1] = temp;
            }
        }
    }
    
    int matchCount = 0;
    for (int i = 0; i < activeTeamsCount / 2; i++) {
        Player* teamA = activeTeams[i];
        Player* teamB = activeTeams[activeTeamsCount - 1 - i];
        
        if (teamA->teamID.substr(0, 1) == teamB->teamID.substr(0, 1)) {
            std::cout << "Skipping match between " << teamA->playerName << " and " 
                      << teamB->playerName << " (same group)\n";
            continue;
        }
        
        Match* newMatch = new Match();
        newMatch->matchID = "M" + std::to_string(completedCount + matchCount + 1);
        newMatch->teamA = teamA->teamID;
        newMatch->teamB = teamB->teamID;
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
                  << teamA->playerName << " vs " 
                  << teamB->playerName << "\n";
        
        teamA->isActive = false;
        teamB->isActive = false;
        matchCount++;
    }
    
    displayBracketVisual(activeTeams, activeTeamsCount);
    delete[] activeTeams;
}

void MatchScheduler::displayBracketVisual(Player** teams, int teamCount) {
    std::cout << "\n=== Visual Bracket for " << currentRound << " ===\n";
    for (int i = 0; i < teamCount / 2; i++) {
        std::cout << std::setw(20) << std::left << teams[i]->playerName 
                  << " ├── vs ─── " 
                  << teams[teamCount - 1 - i]->playerName << "\n";
        if (i < teamCount / 2 - 1) {
            std::cout << std::setw(20) << " " << " │           " << "\n";
        }
    }
    std::cout << "\n";
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
    Match* match = nullptr;
    MatchNode* current = matchQueueHead;
    MatchNode* prev = nullptr;
    while (current) {
        if (current->match->matchID == matchID) {
            match = current->match;
            if (prev) {
                prev->next = current->next;
            } else {
                matchQueueHead = current->next;
            }
            if (current == matchQueueTail) {
                matchQueueTail = prev;
            }
            break;
        }
        prev = current;
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
    
    std::cout << "Enter score for " << getTeamName(match->teamA) << ": ";
    int scoreA;
    std::cin >> scoreA;
    std::cout << "Enter score for " << getTeamName(match->teamB) << ": ";
    int scoreB;
    std::cin >> scoreB;
    
    match->scoreA = scoreA;
    match->scoreB = scoreB;
    match->winner = winner;
    match->status = "completed";
    
    saveMatchResult(matchID, winner, scoreA, scoreB, match->teamA, match->teamB);
    
    for (int i = 0; i < teamCount; i++) {
        if (allTeams[i].teamID == match->teamA) {
            if (allTeams[i].teamID == winner) {
                allTeams[i].wins++;
                allTeams[i].totalScore += scoreA;
                allTeams[i].isActive = true;
                std::cout << allTeams[i].playerName << " advances to the next round!\n";
            } else {
                allTeams[i].losses++;
                allTeams[i].isActive = false;
            }
        } else if (allTeams[i].teamID == match->teamB) {
            if (allTeams[i].teamID == winner) {
                allTeams[i].wins++;
                allTeams[i].totalScore += scoreB;
                allTeams[i].isActive = true;
                std::cout << allTeams[i].playerName << " advances to the next round!\n";
            } else {
                allTeams[i].losses++;
                allTeams[i].isActive = false;
            }
        }
    }
    
    MatchResult matchResult;
    matchResult.matchId = std::stoi(matchID.substr(1));
    matchResult.team1 = match->teamA;
    matchResult.team2 = match->teamB;
    matchResult.winner = match->winner;
    matchResult.loser = (match->winner == match->teamA) ? match->teamB : match->teamA;
    matchResult.team1Score = scoreA;
    matchResult.team2Score = scoreB;
    matchResult.gameMode = "1v1";
    matchResult.matchDate = "2025-06-15";
    matchResult.durationMinutes = 30;

    if (perfManager) {
        perfManager->recordMatchResult(matchResult);
    }
    
    completedMatches[completedCount] = *match;
    completedCount++;
    delete match;
    if (current) delete current;
}

void MatchScheduler::saveMatchResult(const std::string& matchID, const std::string& winner, int scoreA, int scoreB, const std::string& teamA, const std::string& teamB) {
    std::ofstream file("data/results.csv", std::ios::app);
    
    std::ifstream checkFile("data/results.csv");
    bool isEmpty = checkFile.peek() == std::ifstream::traits_type::eof();
    checkFile.close();
    
    if (isEmpty) {
        file << "MatchID,TeamA,TeamB,ScoreA,ScoreB,Winner,Timestamp,Round\n";
    }
    
    time_t now = time(0);
    char* timeStr = ctime(&now);
    std::string timestamp(timeStr);
    timestamp.pop_back();
    
    file << matchID << "," << teamA << "," << teamB << "," 
         << scoreA << "," << scoreB << "," << winner << "," 
         << timestamp << "," << currentRound << "\n";
    
    file.close();
    std::cout << "Match result saved to results.csv\n";
}

void MatchScheduler::displayTournamentBracket() {
    std::cout << "\n=== TOURNAMENT BRACKET VISUALIZATION ===\n";
    
    int active = 0, eliminated = 0;
    
    std::cout << "\n--- ACTIVE TEAMS ---\n";
    for (int i = 0; i < teamCount; i++) {
        if (allTeams[i].isActive) {
            std::cout << "🏆 " << allTeams[i].playerName 
                      << " (Rank: " << allTeams[i].rank
                      << ") W:" << allTeams[i].wins 
                      << " Score:" << allTeams[i].totalScore << "\n";
            active++;
        }
    }
    
    std::cout << "\n--- ELIMINATED TEAMS ---\n";
    for (int i = 0; i < teamCount; i++) {
        if (!allTeams[i].isActive && allTeams[i].losses > 0) {
            std::cout << "❌ " << allTeams[i].playerName 
                      << " (Rank: " << allTeams[i].rank
                      << ") W:" << allTeams[i].wins 
                      << " L:" << allTeams[i].losses << "\n";
            eliminated++;
        }
    }
    
    std::cout << "\nCurrent Round: " << currentRound << "\n";
    std::cout << "Teams Remaining: " << active << "\n";
    
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
    std::cout << "\n=== PLAYER STATISTICS ===\n";
    
    Player* sortedTeams[32];
    for (int i = 0; i < teamCount; i++) {
        sortedTeams[i] = &allTeams[i];
    }
    
    for (int i = 0; i < teamCount - 1; i++) {
        for (int j = 0; j < teamCount - i - 1; j++) {
            if (sortedTeams[j]->wins < sortedTeams[j + 1]->wins || 
                (sortedTeams[j]->wins == sortedTeams[j + 1]->wins && 
                 sortedTeams[j]->totalScore < sortedTeams[j + 1]->totalScore)) {
                Player* temp = sortedTeams[j];
                sortedTeams[j] = sortedTeams[j + 1];
                sortedTeams[j + 1] = temp;
            }
        }
    }
    
    std::cout << "Rank | Player Name             | W | L | Total Score | Status\n";
    std::cout << "-----|-------------------------|---|---|-------------|--------\n";
    
    for (int i = 0; i < teamCount; i++) {
        Player* team = sortedTeams[i];
        std::cout << std::setw(4) << (i + 1) << " | " 
                  << std::setw(23) << team->playerName << " | "
                  << team->wins << " | " << team->losses << " | "
                  << std::setw(11) << team->totalScore << " | "
                  << (team->isActive ? "Active" : "Eliminated") << "\n";
    }
}

std::string MatchScheduler::getTeamName(const std::string& teamID) {
    for (int i = 0; i < teamCount; i++) {
        if (allTeams[i].teamID == teamID) {
            return allTeams[i].playerName;
        }
    }
    return teamID;
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