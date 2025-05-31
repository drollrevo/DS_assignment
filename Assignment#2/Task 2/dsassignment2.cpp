#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

// Player structure to store player information
struct Player {
    string playerID;
    string playerName;
    string university;
    string registrationType; // "early-bird", "regular", "wildcard"
    bool checkedIn;
    
    Player(string id = "", string name = "", string uni = "", string type = "regular") 
        : playerID(id), playerName(name), university(uni), registrationType(type), checkedIn(false) {}
    
    // For priority queue comparison (higher priority = lower number)
    int getPriority() const {
        if (registrationType == "early-bird") return 1;
        if (registrationType == "wildcard") return 2;
        return 3; // regular
    }
};

// Custom comparator for priority queue (higher priority players come first)
struct PlayerComparator {
    bool operator()(const Player& a, const Player& b) {
        return a.getPriority() > b.getPriority();
    }
};

class TournamentRegistration {
private:
    priority_queue<Player, vector<Player>, PlayerComparator> registrationQueue;
    queue<Player> checkInQueue;
    queue<Player> withdrawalQueue;
    vector<Player> registeredPlayers;
    vector<Player> checkedInPlayers;
    int totalCapacity;
    
public:
    TournamentRegistration(int capacity = 100) : totalCapacity(capacity) {}
    
    // Register a new player
    bool registerPlayer(const Player& player) {
        if (registeredPlayers.size() >= totalCapacity) {
            cout << "Registration full! Player " << player.playerName << " added to waiting list.\n";
            return false;
        }
        
        registrationQueue.push(player);
        registeredPlayers.push_back(player);
        
        cout << "Player " << player.playerName << " (" << player.registrationType 
             << ") registered successfully!\n";
        return true;
    }
    
    // Process registrations in priority order
    void processRegistrations() {
        cout << "\n=== Processing Registrations in Priority Order ===\n";
        priority_queue<Player, vector<Player>, PlayerComparator> tempQueue = registrationQueue;
        
        int count = 1;
        while (!tempQueue.empty()) {
            Player p = tempQueue.top();
            tempQueue.pop();
            
            cout << count++ << ". " << p.playerName << " (" << p.university 
                 << ") - " << p.registrationType << " registration\n";
        }
    }
    
    // Player check-in
    bool checkInPlayer(const string& playerID) {
        // Find player in registered list
        for (auto& player : registeredPlayers) {
            if (player.playerID == playerID && !player.checkedIn) {
                player.checkedIn = true;
                checkInQueue.push(player);
                checkedInPlayers.push_back(player);
                
                cout << "Player " << player.playerName << " checked in successfully!\n";
                return true;
            }
        }
        
        cout << "Player ID " << playerID << " not found or already checked in!\n";
        return false;
    }
    
    // Handle player withdrawal
    void withdrawPlayer(const string& playerID, const string& reason = "Personal reasons") {
        bool found = false;
        
        // Remove from registered players and mark as withdrawn
        for (auto it = registeredPlayers.begin(); it != registeredPlayers.end(); ++it) {
            if (it->playerID == playerID) {
                Player withdrawnPlayer = *it;
                withdrawnPlayer.registrationType = "withdrawn";
                withdrawalQueue.push(withdrawnPlayer);
                
                registeredPlayers.erase(it);
                cout << "Player " << withdrawnPlayer.playerName << " withdrawn. Reason: " 
                     << reason << "\n";
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "Player ID " << playerID << " not found in registration!\n";
        }
    }
    
    // Add replacement player (fills withdrawn player's spot)
    void addReplacementPlayer(const Player& replacementPlayer) {
        if (registeredPlayers.size() < totalCapacity) {
            Player newPlayer = replacementPlayer;
            newPlayer.registrationType = "replacement";
            
            registerPlayer(newPlayer);
            cout << "Replacement player " << newPlayer.playerName << " added successfully!\n";
        } else {
            cout << "Cannot add replacement - tournament at full capacity!\n";
        }
    }
    
    // Display current registration statistics
    void displayStats() {
        cout << "\n=== Tournament Registration Statistics ===\n";
        cout << "Total Registered: " << registeredPlayers.size() << "/" << totalCapacity << "\n";
        cout << "Checked In: " << checkedInPlayers.size() << "\n";
        cout << "Pending Check-in: " << (registeredPlayers.size() - checkedInPlayers.size()) << "\n";
        cout << "Withdrawals: " << withdrawalQueue.size() << "\n";
        
        // Count by registration type
        int earlyBird = 0, regular = 0, wildcard = 0, replacement = 0;
        for (const auto& player : registeredPlayers) {
            if (player.registrationType == "early-bird") earlyBird++;
            else if (player.registrationType == "regular") regular++;
            else if (player.registrationType == "wildcard") wildcard++;
            else if (player.registrationType == "replacement") replacement++;
        }
        
        cout << "\nRegistration Types:\n";
        cout << "- Early Bird: " << earlyBird << "\n";
        cout << "- Regular: " << regular << "\n";
        cout << "- Wildcard: " << wildcard << "\n";
        cout << "- Replacement: " << replacement << "\n";
    }
    
    // Display check-in queue
    void displayCheckInQueue() {
        cout << "\n=== Players Checked In (FIFO Order) ===\n";
        queue<Player> tempQueue = checkInQueue;
        int count = 1;
        
        while (!tempQueue.empty()) {
            Player p = tempQueue.front();
            tempQueue.pop();
            cout << count++ << ". " << p.playerName << " (" << p.university << ")\n";
        }
        
        if (count == 1) {
            cout << "No players checked in yet.\n";
        }
    }
    
    // Display withdrawal history
    void displayWithdrawals() {
        cout << "\n=== Withdrawal History ===\n";
        queue<Player> tempQueue = withdrawalQueue;
        int count = 1;
        
        while (!tempQueue.empty()) {
            Player p = tempQueue.front();
            tempQueue.pop();
            cout << count++ << ". " << p.playerName << " (" << p.university << ")\n";
        }
        
        if (count == 1) {
            cout << "No withdrawals recorded.\n";
        }
    }
    
    // Save registration data to CSV file
    void saveToFile(const string& filename) {
        ofstream file(filename);
        file << "PlayerID,PlayerName,University,RegistrationType,CheckedIn\n";
        
        for (const auto& player : registeredPlayers) {
            file << player.playerID << "," << player.playerName << "," 
                 << player.university << "," << player.registrationType << "," 
                 << (player.checkedIn ? "Yes" : "No") << "\n";
        }
        
        file.close();
        cout << "Registration data saved to " << filename << "\n";
    }
};

// Interactive menu system
void displayMenu() {
    cout << "\n=== APU ESPORTS CHAMPIONSHIP - REGISTRATION MENU ===\n";
    cout << "1. Register New Player\n";
    cout << "2. Check-in Player\n";
    cout << "3. Withdraw Player\n";
    cout << "4. Add Replacement Player\n";
    cout << "5. View Registration Statistics\n";
    cout << "6. View Check-in Queue\n";
    cout << "7. View Withdrawal History\n";
    cout << "8. Process Registrations (Priority Order)\n";
    cout << "9. Save Data to File\n";
    cout << "0. Exit\n";
    cout << "Enter your choice: ";
}

// Get player input from user
Player getPlayerInput(const string& playerType = "regular") {
    Player newPlayer;
    
    cout << "\n=== Player Registration Form ===\n";
    cout << "Enter Player ID: ";
    cin >> newPlayer.playerID;
    cin.ignore(); // Clear input buffer
    
    cout << "Enter Player Name: ";
    getline(cin, newPlayer.playerName);
    
    cout << "Enter University: ";
    getline(cin, newPlayer.university);
    
    if (playerType == "regular") {
        cout << "Select Registration Type:\n";
        cout << "1. Early-bird\n";
        cout << "2. Regular\n";
        cout << "3. Wildcard\n";
        cout << "Enter choice (1-3): ";
        
        int typeChoice;
        cin >> typeChoice;
        
        switch (typeChoice) {
            case 1: newPlayer.registrationType = "early-bird"; break;
            case 2: newPlayer.registrationType = "regular"; break;
            case 3: newPlayer.registrationType = "wildcard"; break;
            default: 
                newPlayer.registrationType = "regular";
                cout << "Invalid choice. Set to regular registration.\n";
        }
    } else {
        newPlayer.registrationType = playerType;
    }
    
    return newPlayer;
}

// Interactive demonstration system
void runInteractiveSystem() {
    cout << "=== APU ESPORTS CHAMPIONSHIP - REGISTRATION SYSTEM ===\n";
    cout << "Welcome to the Tournament Registration System!\n";
    
    int capacity;
    cout << "Enter tournament capacity: ";
    cin >> capacity;
    
    TournamentRegistration tournament(capacity);
    int choice;
    
    do {
        displayMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                Player newPlayer = getPlayerInput();
                tournament.registerPlayer(newPlayer);
                break;
            }
            case 2: {
                string playerID;
                cout << "Enter Player ID to check-in: ";
                cin >> playerID;
                tournament.checkInPlayer(playerID);
                break;
            }
            case 3: {
                string playerID, reason;
                cout << "Enter Player ID to withdraw: ";
                cin >> playerID;
                cin.ignore();
                cout << "Enter withdrawal reason: ";
                getline(cin, reason);
                tournament.withdrawPlayer(playerID, reason);
                break;
            }
            case 4: {
                Player replacementPlayer = getPlayerInput("replacement");
                tournament.addReplacementPlayer(replacementPlayer);
                break;
            }
            case 5: {
                tournament.displayStats();
                break;
            }
            case 6: {
                tournament.displayCheckInQueue();
                break;
            }
            case 7: {
                tournament.displayWithdrawals();
                break;
            }
            case 8: {
                tournament.processRegistrations();
                break;
            }
            case 9: {
                string filename;
                cout << "Enter filename (with .csv extension): ";
                cin >> filename;
                tournament.saveToFile(filename);
                break;
            }
            case 0: {
                cout << "Thank you for using the Registration System!\n";
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
}

int main() {
    runInteractiveSystem();
    return 0;
}