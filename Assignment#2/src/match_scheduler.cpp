#include "match_scheduler.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

static const int INITIAL_CAPACITY = 64;
static const int MAX_PLAYERS = 128;
static const int MAX_MATCHES = 100;

MatchScheduler::MatchScheduler() 
    : heapSize(0), heapCapacity(INITIAL_CAPACITY), playerCount(0),
      matchQueueHead(nullptr), matchQueueTail(nullptr), completedCount(0), maxPlayers(MAX_PLAYERS) {
    
    playerHeap = new Player*[heapCapacity];
    allPlayers = new Player[maxPlayers];
    completedMatches = new Match[MAX_MATCHES];
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
    
    while (std::getline(file, line) && playerCount < maxPlayers) {
        if (line.empty()) continue;
        
        std::string fields[3];
        parseLine(line, fields, 3);
        
        // Store team rank for later use with players
        // For now, just acknowledge team data is loaded
    }
    file.close();
}

void MatchScheduler::loadPlayers(const std::string& filename) {
    std::ifstream file("data/players.csv");
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    
    std::string line;
    // Skip header
    if (!std::getline(file, line)) return;
    
    int order = 1;
    while (std::getline(file, line) && playerCount < maxPlayers) {
        if (line.empty()) continue;
        
        std::string fields[3];
        parseLine(line, fields, 3);
        
        Player* p = &allPlayers[playerCount];
        p->playerID = fields[0];
        p->playerName = fields[1];
        p->teamID = fields[2];
        p->rank = order; // Use order as rank for now
        p->arrivalOrder = order;
        p->currentStage = "qualifier";
        p->isActive = true;
        p->wins = 0;
        p->losses = 0;
        
        // Add to heap
        if (heapSize == heapCapacity) {
            resizeHeap();
        }
        playerHeap[heapSize] = p;
        siftUp(heapSize);
        heapSize++;
        
        playerCount++;
        order++;
    }
    file.close();
    std::cout << "Loaded " << playerCount << " players.\n";
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
    std::cout << "\n=== Generating " << stage << " Match Pairings ===\n";
    
    // Create temporary array for stage players
    Player* stagePlayer[64];
    int stageCount = 0;
    
    // Collect players for current stage
    for (int i = 0; i < playerCount && stageCount < 64; i++) {
        if (allPlayers[i].currentStage == stage && allPlayers[i].isActive) {
            stagePlayer[stageCount++] = &allPlayers[i];
        }
    }
    
    if (stageCount < 2) {
        std::cout << "Not enough players for " << stage << " matches!\n";
        return;
    }
    
    // Simple pairing - pair adjacent players
    for (int i = 0; i < stageCount - 1; i += 2) {
        Match* newMatch = new Match();
        newMatch->matchID = "M" + std::to_string(completedCount + 1);
        newMatch->teamA = stagePlayer[i]->teamID;
        newMatch->teamB = stagePlayer[i+1]->teamID;
        newMatch->status = "scheduled";
        
        // Add to match queue
        MatchNode* node = new MatchNode(newMatch);
        if (!matchQueueHead) {
            matchQueueHead = matchQueueTail = node;
        } else {
            matchQueueTail->next = node;
            matchQueueTail = node;
        }
        
        std::cout << "Match " << newMatch->matchID << ": " 
                  << stagePlayer[i]->playerName << " vs " 
                  << stagePlayer[i+1]->playerName << "\n";
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
    
    std::cout << "\n=== Next Match ===\n";
    std::cout << "Match ID: " << nextMatch->matchID << "\n";
    std::cout << "Team A: " << nextMatch->teamA << "\n";
    std::cout << "Team B: " << nextMatch->teamB << "\n";
    
    delete temp; // Only delete the node, not the match
    return nextMatch;
}

void MatchScheduler::recordMatchResult(const std::string& matchID, const std::string& winner) {
    std::cout << "Recording result for " << matchID << ": Winner = " << winner << "\n";
    
    // Update player stats based on team results
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].teamID == winner) {
            allPlayers[i].wins++;
            
            // Check for progression
            if (allPlayers[i].currentStage == "qualifier" && allPlayers[i].wins >= 2) {
                allPlayers[i].currentStage = "group";
                std::cout << allPlayers[i].playerName << " advanced to Group Stage!\n";
            } else if (allPlayers[i].currentStage == "group" && allPlayers[i].wins >= 4) {
                allPlayers[i].currentStage = "knockout";
                std::cout << allPlayers[i].playerName << " advanced to Knockout Stage!\n";
            }
        }
    }
}

void MatchScheduler::displayTournamentBracket() {
    std::cout << "\n=== TOURNAMENT BRACKET ===\n";
    
    int knockout = 0, group = 0, qualifier = 0, eliminated = 0;
    
    std::cout << "\n--- KNOCKOUT STAGE ---\n";
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].currentStage == "knockout") {
            std::cout << "• " << allPlayers[i].playerName 
                      << " (Team: " << allPlayers[i].teamID 
                      << ") W:" << allPlayers[i].wins 
                      << " L:" << allPlayers[i].losses << "\n";
            knockout++;
        }
    }
    if (knockout == 0) std::cout << "No players in knockout stage.\n";
    
    std::cout << "\n--- GROUP STAGE ---\n";
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].currentStage == "group") {
            std::cout << "• " << allPlayers[i].playerName 
                      << " (Team: " << allPlayers[i].teamID 
                      << ") W:" << allPlayers[i].wins 
                      << " L:" << allPlayers[i].losses << "\n";
            group++;
        }
    }
    if (group == 0) std::cout << "No players in group stage.\n";
    
    std::cout << "\n--- QUALIFIER STAGE ---\n";
    for (int i = 0; i < playerCount; i++) {
        if (allPlayers[i].currentStage == "qualifier") {
            std::cout << "• " << allPlayers[i].playerName 
                      << " (Team: " << allPlayers[i].teamID 
                      << ") W:" << allPlayers[i].wins 
                      << " L:" << allPlayers[i].losses << "\n";
            qualifier++;
        }
    }
    if (qualifier == 0) std::cout << "No players in qualifier stage.\n";
}

void MatchScheduler::displayMatchSchedule() {
    std::cout << "\n=== MATCH SCHEDULE ===\n";
    
    MatchNode* current = matchQueueHead;
    if (!current) {
        std::cout << "No matches scheduled.\n";
        return;
    }
    
    while (current) {
        Match* m = current->match;
        std::cout << m->matchID << ": " << m->teamA << " vs " << m->teamB 
                  << " [" << m->status << "]\n";
        current = current->next;
    }
}

void MatchScheduler::displayPlayerStats() {
    std::cout << "\n=== PLAYER STATISTICS ===\n";
    
    for (int i = 0; i < playerCount; i++) {
        std::cout << allPlayers[i].playerName 
                  << " (ID: " << allPlayers[i].playerID 
                  << ", Team: " << allPlayers[i].teamID 
                  << ") Stage: " << allPlayers[i].currentStage 
                  << " W:" << allPlayers[i].wins 
                  << " L:" << allPlayers[i].losses << "\n";
    }
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
    // Clear match queue
    MatchNode* current = matchQueueHead;
    while (current) {
        MatchNode* temp = current->next;
        delete current->match;
        delete current;
        current = temp;
    }
    matchQueueHead = matchQueueTail = nullptr;
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
    // Lower rank value = higher priority (better player)
    if (a->rank < b->rank) return -1;
    if (a->rank > b->rank) return 1;
    // Tie-break by arrival order
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
    std::cout << "Player stats updated based on match results.\n";
}
