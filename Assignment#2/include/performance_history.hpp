#ifndef PERFORMANCE_HISTORY_HPP
#define PERFORMANCE_HISTORY_HPP

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Forward declarations
struct Player;
struct MatchResult;
struct PerformanceNode;
template<typename T> class Stack;
class GameResultLogger;

// Custom Stack template class declaration
template<typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& value);
    };
    Node* top;
    int size;
    int maxSize;

public:
    // Constructor and Destructor
    Stack(int max = 20);
    ~Stack();
    
    // Core stack operations
    void push(const T& value);
    void pop();
    T peek() const;
    bool isEmpty() const;
    int getSize() const;
    void display() const;
    
    // Copy constructor for temporary stack creation
    Stack(const Stack& other);
    Stack& operator=(const Stack& other);
};

// Player structure declaration
struct Player {
    string id;
    string name;
    string university;
    int ranking;
    int wins;
    int losses;
    int totalMatches;
    
    // Constructors
    Player();
    Player(const string& playerId, const string& playerName, const string& uni, int rank);
    
    // Utility methods
    double getWinRate() const;
    void displayInfo() const;
};

// Match Result structure declaration
struct MatchResult {
    string matchId;
    string player1Id;
    string player2Id;
    string winnerId;
    string stage;
    string timestamp;
    int player1Score;
    int player2Score;
    
    // Constructors
    MatchResult();
    MatchResult(const string& id, const string& p1, const string& p2, const string& winner,
                const string& stg, const string& time, int score1, int score2);
    
    // Utility methods
    void displayResult() const;
    bool isValidResult() const;
};

// Performance History Node for linked list
struct PerformanceNode {
    MatchResult result;
    PerformanceNode* next;
    
    PerformanceNode(const MatchResult& res);
};

// Main Game Result Logger class declaration
class GameResultLogger {
private:
    Stack<MatchResult> recentResults;
    Player* players;
    int playerCount;
    int maxPlayers;
    PerformanceNode* historyHead;
    
    // Private helper methods
    void createSamplePlayers();
    MatchResult parseResultLine(const string& line);
    void addToHistory(const MatchResult& result);
    void updatePlayerStats(const MatchResult& result);
    void saveResultToFile(const MatchResult& result);
    void clearHistory();
    string generateTimestamp() const;
    
public:
    // Constructor and Destructor
    GameResultLogger(int maxPlayerCount = 100);
    ~GameResultLogger();
    
    // File I/O operations
    void loadPlayers();
    void loadResults();
    void savePlayersToFile() const;
    
    // Core functionality
    void logMatchResult(const MatchResult& result);
    void simulateMatchResult(const string& player1Id, const string& player2Id, const string& stage);
    
    // Display methods
    void displayRecentResults();
    void displayPlayerPerformance(const string& playerId);
    void displayAllPlayerStats();
    void displayMatchHistory(int limit = 10);
    
    // Player management
    bool addPlayer(const Player& player);
    bool removePlayer(const string& playerId);
    Player* findPlayer(const string& playerId);
    string getPlayerName(const string& playerId);
    int getPlayerRanking(const string& playerId);
    
    // Statistics and analysis
    void generateTournamentReport();
    void displayTopPerformers(int count = 5);
    void displayStageStatistics(const string& stage);
    
    // Integration methods
    bool isPlayerActive(const string& playerId);
    int getTotalMatches() const;
    int getPlayerCount() const;
    
    // Utility methods
    void demoSystem();
    void clearAllData();
    bool exportResultsToCSV(const string& filename) const;
    bool importResultsFromCSV(const string& filename);
    
    // System status
    void displaySystemStatus();
    void validateDataIntegrity();
};

// Utility functions (global scope)
string trimWhitespace(const string& str);
bool isValidPlayerId(const string& playerId);
bool isValidStage(const string& stage);
string getCurrentTimestamp();
void displayHeader(const string& title);
void displaySeparator(int length = 50);

// Constants
const int MAX_RECENT_RESULTS = 20;
const int MAX_HISTORY_DISPLAY = 10;
const string DEFAULT_TIMESTAMP = "2025-06-02 14:30:00";
const string VALID_STAGES[] = {"Qualifier", "Group Stage", "Knockout", "Semifinal", "Final"};
const int NUM_VALID_STAGES = 5;

// Template implementation (must be in header for templates)
template<typename T>
Stack<T>::Node::Node(const T& value) : data(value), next(nullptr) {}

template<typename T>
Stack<T>::Stack(int max) : top(nullptr), size(0), maxSize(max) {}

template<typename T>
Stack<T>::~Stack() {
    while (!isEmpty()) {
        pop();
    }
}

template<typename T>
Stack<T>::Stack(const Stack& other) : top(nullptr), size(0), maxSize(other.maxSize) {
    // Create temporary array to reverse order
    T* temp = new T[other.size];
    Node* current = other.top;
    int index = 0;
    
    // Copy elements to array
    while (current && index < other.size) {
        temp[index++] = current->data;
        current = current->next;
    }
    
    // Push elements in reverse order to maintain stack order
    for (int i = index - 1; i >= 0; i--) {
        push(temp[i]);
    }
    
    delete[] temp;
}

template<typename T>
Stack<T>& Stack<T>::operator=(const Stack& other) {
    if (this != &other) {
        // Clear current stack
        while (!isEmpty()) {
            pop();
        }
        
        maxSize = other.maxSize;
        
        // Copy elements
        T* temp = new T[other.size];
        Node* current = other.top;
        int index = 0;
        
        while (current && index < other.size) {
            temp[index++] = current->data;
            current = current->next;
        }
        
        for (int i = index - 1; i >= 0; i--) {
            push(temp[i]);
        }
        
        delete[] temp;
    }
    return *this;
}

template<typename T>
void Stack<T>::push(const T& value) {
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

template<typename T>
void Stack<T>::pop() {
    if (!isEmpty()) {
        Node* temp = top;
        top = top->next;
        delete temp;
        size--;
    }
}

template<typename T>
T Stack<T>::peek() const {
    if (!isEmpty()) return top->data;
    return T();
}

template<typename T>
bool Stack<T>::isEmpty() const {
    return top == nullptr;
}

template<typename T>
int Stack<T>::getSize() const {
    return size;
}

template<typename T>
void Stack<T>::display() const {
    Node* current = top;
    while (current) {
        cout << current->data << endl;
        current = current->next;
    }
}

#endif 