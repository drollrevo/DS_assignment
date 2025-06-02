#include "performance_history.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;

// Custom Stack implementation for recent results
template<typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value) : data(value), next(nullptr) {}
    };
    Node* top;
    int size;
    int maxSize;

public:
    Stack(int max = 20) : top(nullptr), size(0), maxSize(max) {}
    
    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }
    
    void push(const T& value) {
        if (size >= maxSize) {
            // Remove oldest entry (bottom of stack)
            Node* current = top;
            Node* prev = nullptr;
            while (current && current->next) {
                prev = current;
                current = current->next;
            }
            if (current) {
                if (prev) prev->next = nullptr;
                else top = nullptr;
                delete current;
                size--;
            }
        }
        
        Node* newNode = new Node(value);
        newNode->next = top;
        top = newNode;
        size++;
    }
    
    void pop() {
        if (!isEmpty()) {
            Node* temp = top;
            top = top->next;
            delete temp;
            size--;
        }
    }
    
    T peek() const {
        if (!isEmpty()) return top->data;
        return T();
    }
    
    bool isEmpty() const { return top == nullptr; }
    int getSize() const { return size; }
    
    void display() const {
        Node* current = top;
        while (current) {
            cout << current->data << endl;
            current = current->next;
        }
    }
};

// Player structure
struct Player {
    string id;
    string name;
    string university;
    int ranking;
    int wins;
    int losses;
    int totalMatches;
    
    Player() : ranking(0), wins(0), losses(0), totalMatches(0) {}
    
    Player(const string& playerId, const string& playerName, const string& uni, int rank)
        : id(playerId), name(playerName), university(uni), ranking(rank), wins(0), losses(0), totalMatches(0) {}
};

// Match Result structure
struct MatchResult {
    string matchId;
    string player1Id;
    string player2Id;
    string winnerId;
    string stage;
    string timestamp;
    int player1Score;
    int player2Score;
    
    MatchResult() : player1Score(0), player2Score(0) {}
    
    MatchResult(const string& id, const string& p1, const string& p2, const string& winner,
                const string& stg, const string& time, int score1, int score2)
        : matchId(id), player1Id(p1), player2Id(p2), winnerId(winner), stage(stg), 
          timestamp(time), player1Score(score1), player2Score(score2) {}
};

// Performance History Node for linked list
struct PerformanceNode {
    MatchResult result;
    PerformanceNode* next;
    
    PerformanceNode(const MatchResult& res) : result(res), next(nullptr) {}
};

// Main Game Result Logger class
class GameResultLogger {
private:
    Stack<MatchResult> recentResults;
    Player* players;
    int playerCount;
    int maxPlayers;
    PerformanceNode* historyHead;
    
public:
    GameResultLogger(int maxPlayerCount = 100) : maxPlayers(maxPlayerCount), playerCount(0), historyHead(nullptr) {
        players = new Player[maxPlayers];
        loadPlayers();
        loadResults();
    }
    
    ~GameResultLogger() {
        delete[] players;
        clearHistory();
    }
    
    void loadPlayers() {
        ifstream file("players.csv");
        if (!file.is_open()) {
            cout << "Warning: players.csv not found. Creating sample data...\n";
            createSamplePlayers();
            return;
        }
        
        string line;
        getline(file, line); // Skip header
        
        while (getline(file, line) && playerCount < maxPlayers) {
            size_t pos = 0;
            string tokens[4];
            int tokenCount = 0;
            
            // Simple CSV parsing
            while ((pos = line.find(',')) != string::npos && tokenCount < 3) {
                tokens[tokenCount++] = line.substr(0, pos);
                line.erase(0, pos + 1);
            }
            tokens[tokenCount] = line;
            
            if (tokenCount >= 3) {
                players[playerCount] = Player(tokens[0], tokens[1], tokens[2], 
                                            (tokenCount > 3 ? stoi(tokens[3]) : playerCount + 1));
                playerCount++;
            }
        }
        file.close();
        cout << "Loaded " << playerCount << " players from CSV.\n";
    }
    
    void createSamplePlayers() {
        string sampleData[][4] = {
            {"P001", "Alex Chen", "APU", "1"},
            {"P002", "Sarah Wong", "APU", "2"},
            {"P003", "Mike Johnson", "UTM", "3"},
            {"P004", "Lisa Zhang", "UM", "4"},
            {"P005", "David Kim", "APU", "5"}
        };
        
        for (int i = 0; i < 5 && i < maxPlayers; i++) {
            players[playerCount] = Player(sampleData[i][0], sampleData[i][1], 
                                        sampleData[i][2], stoi(sampleData[i][3]));
            playerCount++;
        }
        cout << "Created " << playerCount << " sample players.\n";
    }
    
    void loadResults() {
        ifstream file("results.csv");
        if (!file.is_open()) {
            cout << "Warning: results.csv not found. No previous results loaded.\n";
            return;
        }
        
        string line;
        getline(file, line); // Skip header
        
        while (getline(file, line)) {
            MatchResult result = parseResultLine(line);
            if (!result.matchId.empty()) {
                addToHistory(result);
                updatePlayerStats(result);
            }
        }
        file.close();
        cout << "Loaded previous results from CSV.\n";
    }
    
    MatchResult parseResultLine(const string& line) {
        MatchResult result;
        size_t pos = 0;
        string tokens[8];
        int tokenCount = 0;
        string tempLine = line;
        
        while ((pos = tempLine.find(',')) != string::npos && tokenCount < 7) {
            tokens[tokenCount++] = tempLine.substr(0, pos);
            tempLine.erase(0, pos + 1);
        }
        tokens[tokenCount] = tempLine;
        
        if (tokenCount >= 6) {
            result.matchId = tokens[0];
            result.player1Id = tokens[1];
            result.player2Id = tokens[2];
            result.winnerId = tokens[3];
            result.stage = tokens[4];
            result.timestamp = tokens[5];
            result.player1Score = (tokenCount > 6) ? stoi(tokens[6]) : 0;
            result.player2Score = (tokenCount > 7) ? stoi(tokens[7]) : 0;
        }
        
        return result;
    }
    
    void logMatchResult(const MatchResult& result) {
        // Add to recent results stack
        recentResults.push(result);
        
        // Add to performance history
        addToHistory(result);
        
        // Update player statistics
        updatePlayerStats(result);
        
        // Save to file
        saveResultToFile(result);
        
        cout << "\n=== MATCH RESULT LOGGED ===\n";
        cout << "Match ID: " << result.matchId << "\n";
        cout << "Stage: " << result.stage << "\n";
        cout << "Players: " << getPlayerName(result.player1Id) << " vs " 
             << getPlayerName(result.player2Id) << "\n";
        cout << "Score: " << result.player1Score << " - " << result.player2Score << "\n";
        cout << "Winner: " << getPlayerName(result.winnerId) << "\n";
        cout << "Time: " << result.timestamp << "\n";
        cout << "==========================\n";
    }
    
    void addToHistory(const MatchResult& result) {
        PerformanceNode* newNode = new PerformanceNode(result);
        newNode->next = historyHead;
        historyHead = newNode;
    }
    
    void updatePlayerStats(const MatchResult& result) {
        for (int i = 0; i < playerCount; i++) {
            if (players[i].id == result.player1Id) {
                players[i].totalMatches++;
                if (result.winnerId == result.player1Id) {
                    players[i].wins++;
                } else {
                    players[i].losses++;
                }
            }
            if (players[i].id == result.player2Id) {
                players[i].totalMatches++;
                if (result.winnerId == result.player2Id) {
                    players[i].wins++;
                } else {
                    players[i].losses++;
                }
            }
        }
    }
    
    void saveResultToFile(const MatchResult& result) {
        ofstream file("results.csv", ios::app);
        if (file.is_open()) {
            file << result.matchId << "," << result.player1Id << "," << result.player2Id << ","
                 << result.winnerId << "," << result.stage << "," << result.timestamp << ","
                 << result.player1Score << "," << result.player2Score << "\n";
            file.close();
        }
    }
    
    void displayRecentResults() {
        cout << "\n=== RECENT MATCH RESULTS ===\n";
        if (recentResults.isEmpty()) {
            cout << "No recent results available.\n";
            return;
        }
        
        cout << "Last " << recentResults.getSize() << " matches:\n";
        cout << "----------------------------------------\n";
        
        Stack<MatchResult> temp = recentResults;
        int count = 1;
        while (!temp.isEmpty()) {
            MatchResult result = temp.peek();
            cout << count++ << ". " << result.matchId << " [" << result.stage << "]\n";
            cout << "   " << getPlayerName(result.player1Id) << " " << result.player1Score 
                 << " - " << result.player2Score << " " << getPlayerName(result.player2Id) << "\n";
            cout << "   Winner: " << getPlayerName(result.winnerId) << " | " << result.timestamp << "\n\n";
            temp.pop();
        }
        cout << "============================\n";
    }
    
    void displayPlayerPerformance(const string& playerId) {
        cout << "\n=== PLAYER PERFORMANCE ===\n";
        
        // Find player stats
        Player* player = nullptr;
        for (int i = 0; i < playerCount; i++) {
            if (players[i].id == playerId) {
                player = &players[i];
                break;
            }
        }
        
        if (!player) {
            cout << "Player not found!\n";
            return;
        }
        
        cout << "Player: " << player->name << " (" << player->id << ")\n";
        cout << "University: " << player->university << "\n";
        cout << "Ranking: " << player->ranking << "\n";
        cout << "Total Matches: " << player->totalMatches << "\n";
        cout << "Wins: " << player->wins << "\n";
        cout << "Losses: " << player->losses << "\n";
        
        if (player->totalMatches > 0) {
            double winRate = (double)player->wins / player->totalMatches * 100;
            cout << "Win Rate: " << winRate << "%\n";
        }
        
        cout << "\nMatch History:\n";
        cout << "----------------------------------------\n";
        
        PerformanceNode* current = historyHead;
        int matchCount = 0;
        while (current && matchCount < 10) {
            if (current->result.player1Id == playerId || current->result.player2Id == playerId) {
                cout << current->result.matchId << " [" << current->result.stage << "]\n";
                cout << "  " << getPlayerName(current->result.player1Id) << " " 
                     << current->result.player1Score << " - " << current->result.player2Score 
                     << " " << getPlayerName(current->result.player2Id) << "\n";
                cout << "  Result: " << (current->result.winnerId == playerId ? "WIN" : "LOSS") 
                     << " | " << current->result.timestamp << "\n\n";
                matchCount++;
            }
            current = current->next;
        }
        cout << "==========================\n";
    }
    
    void displayAllPlayerStats() {
        cout << "\n=== ALL PLAYER STATISTICS ===\n";
        cout << "ID\tName\t\tUniversity\tRanking\tMatches\tWins\tLosses\tW%\n";
        cout << "--------------------------------------------------------------------------------\n";
        
        for (int i = 0; i < playerCount; i++) {
            double winRate = (players[i].totalMatches > 0) ? 
                            (double)players[i].wins / players[i].totalMatches * 100 : 0;
            
            cout << players[i].id << "\t" << players[i].name.substr(0, 12) << "\t" 
                 << players[i].university << "\t\t" << players[i].ranking << "\t"
                 << players[i].totalMatches << "\t" << players[i].wins << "\t" 
                 << players[i].losses << "\t" << (int)winRate << "%\n";
        }
        cout << "==============================\n";
    }
    
    // Integration with Match Scheduler
    void simulateMatchResult(const string& player1Id, const string& player2Id, const string& stage) {
        // Generate match ID
        static int matchCounter = 1000;
        string matchId = "M" + to_string(matchCounter++);
        
        // Simulate match result (simple random-like based on rankings)
        string winnerId;
        int score1, score2;
        
        int rank1 = getPlayerRanking(player1Id);
        int rank2 = getPlayerRanking(player2Id);
        
        // Lower ranking number = better player
        if (rank1 < rank2) {
            winnerId = player1Id;
            score1 = 3; score2 = 1;
        } else {
            winnerId = player2Id;
            score1 = 1; score2 = 3;
        }
        
        // Create timestamp
        string timestamp = "2025-06-02 14:30:00";
        
        MatchResult result(matchId, player1Id, player2Id, winnerId, stage, timestamp, score1, score2);
        logMatchResult(result);
    }
    
    string getPlayerName(const string& playerId) {
        for (int i = 0; i < playerCount; i++) {
            if (players[i].id == playerId) {
                return players[i].name;
            }
        }
        return "Unknown";
    }
    
    int getPlayerRanking(const string& playerId) {
        for (int i = 0; i < playerCount; i++) {
            if (players[i].id == playerId) {
                return players[i].ranking;
            }
        }
        return 999;
    }
    
    void clearHistory() {
        while (historyHead) {
            PerformanceNode* temp = historyHead;
            historyHead = historyHead->next;
            delete temp;
        }
    }
    
    void demoSystem() {
        cout << "\n========== GAME RESULT LOGGING SYSTEM DEMO ==========\n";
        
        // Show current players
        displayAllPlayerStats();
        
        // Simulate some matches
        cout << "\nSimulating new matches...\n";
        simulateMatchResult("P001", "P002", "Qualifier");
        simulateMatchResult("P003", "P004", "Qualifier");
        simulateMatchResult("P001", "P005", "Group Stage");
        
        // Show recent results
        displayRecentResults();
        
        // Show individual player performance
        displayPlayerPerformance("P001");
        
        // Show updated stats
        displayAllPlayerStats();
    }
};

// Main function demonstrating the system
int main() {
    cout << "=== APUEC GAME RESULT LOGGING SYSTEM ===\n";
    cout << "Task 4: Game Result Logging & Performance History\n";
    cout << "==========================================\n";
    
    GameResultLogger logger;
    
    int choice;
    do {
        cout << "\n--- MENU ---\n";
        cout << "1. View Recent Results\n";
        cout << "2. View All Player Stats\n";
        cout << "3. View Player Performance\n";
        cout << "4. Simulate Match Result\n";
        cout << "5. Run Demo\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                logger.displayRecentResults();
                break;
            case 2:
                logger.displayAllPlayerStats();
                break;
            case 3: {
                string playerId;
                cout << "Enter Player ID: ";
                cin >> playerId;
                logger.displayPlayerPerformance(playerId);
                break;
            }
            case 4: {
                string p1, p2, stage;
                cout << "Enter Player 1 ID: ";
                cin >> p1;
                cout << "Enter Player 2 ID: ";
                cin >> p2;
                cout << "Enter Stage (Qualifier/Group/Knockout): ";
                cin >> stage;
                logger.simulateMatchResult(p1, p2, stage);
                break;
            }
            case 5:
                logger.demoSystem();
                break;
            case 0:
                cout << "Thank you for using APUEC Result Logging System!\n";
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);
    
    return 0;
}