#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

// Player structure for match scheduling
struct MatchPlayer {
    string playerID;
    string playerName;
    string university;
    int ranking;
    int wins;
    int losses;
    string currentStage; // "qualifier", "group", "knockout", "eliminated"
    bool isActive;
    
    MatchPlayer(string id = "", string name = "", string uni = "", int rank = 0) 
        : playerID(id), playerName(name), university(uni), ranking(rank), 
          wins(0), losses(0), currentStage("qualifier"), isActive(true) {}
    
    // Calculate match priority (higher ranking + fewer losses = higher priority)
    int getMatchPriority() const {
        return (ranking * 100) - (losses * 10) + wins;
    }
    
    double getWinRate() const {
        if (wins + losses == 0) return 0.0;
        return (double)wins / (wins + losses);
    }
};

// Match structure
struct Match {
    string matchID;
    MatchPlayer player1;
    MatchPlayer player2;
    string stage;
    string status; // "scheduled", "ongoing", "completed"
    string winner;
    string matchTime;
    
    Match(string id, MatchPlayer p1, MatchPlayer p2, string stg) 
        : matchID(id), player1(p1), player2(p2), stage(stg), 
          status("scheduled"), winner(""), matchTime("") {}
    
    // Priority for match processing (higher stage priority)
    int getSchedulePriority() const {
        if (stage == "knockout") return 3;
        if (stage == "group") return 2;
        return 1; // qualifier
    }
};

// Comparator for player priority queue (higher priority first)
struct PlayerPriorityComparator {
    bool operator()(const MatchPlayer& a, const MatchPlayer& b) {
        return a.getMatchPriority() < b.getMatchPriority();
    }
};

// Comparator for match priority queue
struct MatchPriorityComparator {
    bool operator()(const Match& a, const Match& b) {
        return a.getSchedulePriority() < b.getSchedulePriority();
    }
};

class MatchScheduler {
private:
    priority_queue<MatchPlayer, vector<MatchPlayer>, PlayerPriorityComparator> playerQueue;
    priority_queue<Match, vector<Match>, MatchPriorityComparator> matchQueue;
    vector<MatchPlayer> allPlayers;
    vector<Match> completedMatches;
    vector<Match> scheduledMatches;
    int matchCounter;
    
public:
    MatchScheduler() : matchCounter(1) {}
    
    // Add player to the tournament
    bool addPlayer(const MatchPlayer& player) {
        allPlayers.push_back(player);
        playerQueue.push(player);
        cout << "Player " << player.playerName << " (Rank: " << player.ranking 
             << ") added to tournament!\n";
        return true;
    }
    
    // Generate fair match pairings based on rankings
    void generateMatchPairings(const string& stage) {
        cout << "\n=== Generating " << stage << " Match Pairings ===\n";
        
        vector<MatchPlayer> availablePlayers;
        
        // Get players for current stage
        for (const auto& player : allPlayers) {
            if (player.currentStage == stage && player.isActive) {
                availablePlayers.push_back(player);
            }
        }
        
        if (availablePlayers.size() < 2) {
            cout << "Not enough players for " << stage << " matches!\n";
            return;
        }
        
        // Sort by ranking for fair pairing
        sort(availablePlayers.begin(), availablePlayers.end(), 
             [](const MatchPlayer& a, const MatchPlayer& b) {
                 return a.ranking < b.ranking;
             });
        
        // Create matches pairing similar ranked players
        for (int i = 0; i < availablePlayers.size() - 1; i += 2) {
            string matchID = "M" + to_string(matchCounter++);
            Match newMatch(matchID, availablePlayers[i], availablePlayers[i+1], stage);
            
            matchQueue.push(newMatch);
            scheduledMatches.push_back(newMatch);
            
            cout << "Match " << matchID << ": " << availablePlayers[i].playerName 
                 << " (Rank " << availablePlayers[i].ranking << ") vs " 
                 << availablePlayers[i+1].playerName 
                 << " (Rank " << availablePlayers[i+1].ranking << ")\n";
        }
    }
    
    // Process next scheduled match
    Match getNextMatch() {
        if (matchQueue.empty()) {
            cout << "No matches scheduled!\n";
            return Match("", MatchPlayer(), MatchPlayer(), "");
        }
        
        Match nextMatch = matchQueue.top();
        matchQueue.pop();
        
        cout << "\n=== Next Match to Process ===\n";
        cout << "Match ID: " << nextMatch.matchID << "\n";
        cout << "Stage: " << nextMatch.stage << "\n";
        cout << "Player 1: " << nextMatch.player1.playerName 
             << " (Rank: " << nextMatch.player1.ranking << ")\n";
        cout << "Player 2: " << nextMatch.player2.playerName 
             << " (Rank: " << nextMatch.player2.ranking << ")\n";
        
        return nextMatch;
    }
    
    // Record match result and update player progression
    void recordMatchResult(const string& matchID, const string& winnerID) {
        // Find the match in scheduled matches
        for (auto& match : scheduledMatches) {
            if (match.matchID == matchID && match.status == "scheduled") {
                match.status = "completed";
                
                // Determine winner and update player stats
                if (match.player1.playerID == winnerID) {
                    match.winner = match.player1.playerName;
                    updatePlayerResult(match.player1.playerID, true);
                    updatePlayerResult(match.player2.playerID, false);
                } else if (match.player2.playerID == winnerID) {
                    match.winner = match.player2.playerName;
                    updatePlayerResult(match.player2.playerID, true);
                    updatePlayerResult(match.player1.playerID, false);
                } else {
                    cout << "Invalid winner ID!\n";
                    return;
                }
                
                completedMatches.push_back(match);
                cout << "Match " << matchID << " completed. Winner: " << match.winner << "\n";
                
                // Check for stage progression
                checkStageProgression(match.stage);
                return;
            }
        }
        
        cout << "Match " << matchID << " not found or already completed!\n";
    }
    
    // Update player statistics
    void updatePlayerResult(const string& playerID, bool won) {
        for (auto& player : allPlayers) {
            if (player.playerID == playerID) {
                if (won) {
                    player.wins++;
                } else {
                    player.losses++;
                    
                    // Eliminate player if too many losses
                    if (player.losses >= 2 && player.currentStage != "knockout") {
                        player.isActive = false;
                        player.currentStage = "eliminated";
                        cout << "Player " << player.playerName << " eliminated from tournament!\n";
                    }
                }
                break;
            }
        }
    }
    
    // Check and manage stage progression
    void checkStageProgression(const string& currentStage) {
        if (currentStage == "qualifier") {
            // Move winners to group stage
            for (auto& player : allPlayers) {
                if (player.currentStage == "qualifier" && player.wins >= 2 && player.isActive) {
                    player.currentStage = "group";
                    cout << "Player " << player.playerName << " advanced to Group Stage!\n";
                }
            }
        } else if (currentStage == "group") {
            // Move winners to knockout stage
            for (auto& player : allPlayers) {
                if (player.currentStage == "group" && player.wins >= 3 && player.isActive) {
                    player.currentStage = "knockout";
                    cout << "Player " << player.playerName << " advanced to Knockout Stage!\n";
                }
            }
        }
    }
    
    // Display tournament bracket
    void displayTournamentBracket() {
        cout << "\n=== TOURNAMENT BRACKET ===\n";
        
        // Group players by stage
        vector<MatchPlayer> qualifiers, groupStage, knockoutStage, eliminated;
        
        for (const auto& player : allPlayers) {
            if (player.currentStage == "qualifier") qualifiers.push_back(player);
            else if (player.currentStage == "group") groupStage.push_back(player);
            else if (player.currentStage == "knockout") knockoutStage.push_back(player);
            else if (player.currentStage == "eliminated") eliminated.push_back(player);
        }
        
        cout << "\n--- KNOCKOUT STAGE ---\n";
        displayStageSection(knockoutStage);
        
        cout << "\n--- GROUP STAGE ---\n";
        displayStageSection(groupStage);
        
        cout << "\n--- QUALIFIER STAGE ---\n";
        displayStageSection(qualifiers);
        
        cout << "\n--- ELIMINATED ---\n";
        displayStageSection(eliminated);
    }
    
    // Helper function to display stage section
    void displayStageSection(const vector<MatchPlayer>& players) {
        if (players.empty()) {
            cout << "No players in this stage.\n";
            return;
        }
        
        for (const auto& player : players) {
            cout << "• " << player.playerName << " (" << player.university 
                 << ") - Rank: " << player.ranking 
                 << " | W:" << player.wins << " L:" << player.losses 
                 << " | WR: " << fixed << setprecision(2) << (player.getWinRate() * 100) << "%\n";
        }
    }
    
    // Display match schedule
    void displayMatchSchedule() {
        cout << "\n=== MATCH SCHEDULE ===\n";
        
        if (scheduledMatches.empty()) {
            cout << "No matches scheduled.\n";
            return;
        }
        
        cout << left << setw(8) << "Match ID" << setw(20) << "Player 1" 
             << setw(20) << "Player 2" << setw(12) << "Stage" 
             << setw(12) << "Status" << "Winner\n";
        cout << string(80, '-') << "\n";
        
        for (const auto& match : scheduledMatches) {
            cout << left << setw(8) << match.matchID 
                 << setw(20) << match.player1.playerName 
                 << setw(20) << match.player2.playerName 
                 << setw(12) << match.stage 
                 << setw(12) << match.status 
                 << match.winner << "\n";
        }
    }
    
    // Display player statistics
    void displayPlayerStats() {
        cout << "\n=== PLAYER STATISTICS ===\n";
        
        // Sort players by ranking
        vector<MatchPlayer> sortedPlayers = allPlayers;
        sort(sortedPlayers.begin(), sortedPlayers.end(), 
             [](const MatchPlayer& a, const MatchPlayer& b) {
                 return a.ranking < b.ranking;
             });
        
        cout << left << setw(15) << "Player Name" << setw(20) << "University" 
             << setw(6) << "Rank" << setw(8) << "Stage" 
             << setw(4) << "W" << setw(4) << "L" 
             << setw(8) << "Win%" << "Status\n";
        cout << string(75, '-') << "\n";
        
        for (const auto& player : sortedPlayers) {
            cout << left << setw(15) << player.playerName 
                 << setw(20) << player.university 
                 << setw(6) << player.ranking 
                 << setw(8) << player.currentStage 
                 << setw(4) << player.wins 
                 << setw(4) << player.losses 
                 << setw(8) << fixed << setprecision(1) << (player.getWinRate() * 100) << "%" 
                 << (player.isActive ? "Active" : "Eliminated") << "\n";
        }
    }
    
    // Save tournament data to file
    void saveToFile(const string& filename) {
        ofstream file(filename);
        file << "MatchID,Player1,Player2,Stage,Status,Winner\n";
        
        for (const auto& match : scheduledMatches) {
            file << match.matchID << "," << match.player1.playerName << "," 
                 << match.player2.playerName << "," << match.stage << "," 
                 << match.status << "," << match.winner << "\n";
        }
        
        file.close();
        cout << "Match data saved to " << filename << "\n";
    }
    
    // Get tournament statistics
    void displayTournamentStats() {
        cout << "\n=== TOURNAMENT STATISTICS ===\n";
        cout << "Total Players: " << allPlayers.size() << "\n";
        cout << "Total Matches: " << scheduledMatches.size() << "\n";
        cout << "Completed Matches: " << completedMatches.size() << "\n";
        cout << "Pending Matches: " << (scheduledMatches.size() - completedMatches.size()) << "\n";
        
        int activeCount = 0, eliminatedCount = 0;
        for (const auto& player : allPlayers) {
            if (player.isActive) activeCount++;
            else eliminatedCount++;
        }
        
        cout << "Active Players: " << activeCount << "\n";
        cout << "Eliminated Players: " << eliminatedCount << "\n";
    }
};

// Menu system
void displayMenu() {
    cout << "\n=== APU ESPORTS CHAMPIONSHIP - MATCH SCHEDULER ===\n";
    cout << "1. Add Player to Tournament\n";
    cout << "2. Generate Match Pairings\n";
    cout << "3. Get Next Match\n";
    cout << "4. Record Match Result\n";
    cout << "5. Display Tournament Bracket\n";
    cout << "6. Display Match Schedule\n";
    cout << "7. Display Player Statistics\n";
    cout << "8. Display Tournament Statistics\n";
    cout << "9. Save Data to File\n";
    cout << "0. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    MatchScheduler scheduler;
    int choice;
    
    cout << "=== APU ESPORTS CHAMPIONSHIP - MATCH SCHEDULING SYSTEM ===\n";
    cout << "Welcome to the Match Scheduling & Player Progression System!\n";
    
    do {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                string id, name, uni;
                int ranking;
                
                cout << "Enter Player ID: ";
                cin >> id;
                cin.ignore();
                cout << "Enter Player Name: ";
                getline(cin, name);
                cout << "Enter University: ";
                getline(cin, uni);
                cout << "Enter Player Ranking (1-100): ";
                cin >> ranking;
                
                MatchPlayer newPlayer(id, name, uni, ranking);
                scheduler.addPlayer(newPlayer);
                break;
            }
            case 2: {
                string stage;
                cout << "Enter stage (qualifier/group/knockout): ";
                cin >> stage;
                scheduler.generateMatchPairings(stage);
                break;
            }
            case 3: {
                scheduler.getNextMatch();
                break;
            }
            case 4: {
                string matchID, winnerID;
                cout << "Enter Match ID: ";
                cin >> matchID;
                cout << "Enter Winner Player ID: ";
                cin >> winnerID;
                scheduler.recordMatchResult(matchID, winnerID);
                break;
            }
            case 5: {
                scheduler.displayTournamentBracket();
                break;
            }
            case 6: {
                scheduler.displayMatchSchedule();
                break;
            }
            case 7: {
                scheduler.displayPlayerStats();
                break;
            }
            case 8: {
                scheduler.displayTournamentStats();
                break;
            }
            case 9: {
                string filename;
                cout << "Enter filename (with .csv extension): ";
                cin >> filename;
                scheduler.saveToFile(filename);
                break;
            }
            case 0: {
                cout << "Thank you for using the Match Scheduling System!\n";
                break;
            }
            default: {
                cout << "Invalid choice! Please try again.\n";
                break;
            }
        }
        
        if (choice != 0) {
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
        }
        
    } while (choice != 0);
    
    return 0;
}