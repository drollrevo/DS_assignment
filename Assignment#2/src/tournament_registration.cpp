#include "tournament_registration.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

TournamentRegistration::TournamentRegistration() 
    : regHead(nullptr), front(-1), rear(-1), queueCount(0),
      withdrawalTop(nullptr), allTeams(nullptr), teamCount(0), teamCapacity(10),
      registeredTeams(nullptr), registeredCount(0), registeredCapacity(10),
      arrivalCounter(0), regLog(nullptr)
{
    allTeams = new Team[teamCapacity];
    registeredTeams = new Team*[registeredCapacity];
    
    // Initialize circular queue
    for (int i = 0; i < CIRCULAR_SIZE; i++) {
        circularQueue[i].next = (i + 1) % CIRCULAR_SIZE;
    }
    
    // Open registration log
    regLog = std::fopen("data/registration_log.csv", "w");
    if (regLog) {
        std::fprintf(regLog, "TeamID,TeamName,RegistrationType,Timestamp,Action\n");
        std::fflush(regLog);
    }
}

TournamentRegistration::~TournamentRegistration() {
    clearPriorityQueue();
    clearWithdrawalStack();
    delete[] allTeams;
    delete[] registeredTeams;
    if (regLog) std::fclose(regLog);
}

void TournamentRegistration::loadTeams(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }
    
    std::string line;
    if (!std::getline(file, line)) return; // Skip header
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        if (teamCount >= teamCapacity) {
            resizeTeamArray();
        }
        
        std::string fields[3];
        parseLine(line, fields, 3);
        
        allTeams[teamCount].teamID = fields[0];
        allTeams[teamCount].teamName = fields[1];
        allTeams[teamCount].rank = std::stoi(fields[2]);
        
        // Set early-bird and wildcard flags based on rank
        if (allTeams[teamCount].rank <= 50) {
            allTeams[teamCount].isEarlyBird = true;
        }
        if (allTeams[teamCount].rank >= 150) {
            allTeams[teamCount].isWildcard = true;
        }
        
        teamCount++;
    }
    file.close();
    std::cout << "Loaded " << teamCount << " teams from " << filename << std::endl;
}

void TournamentRegistration::saveRegistrationLog() {
    if (!regLog) return;
    
    std::cout << "Registration log saved to data/registration_log.csv" << std::endl;
}

bool TournamentRegistration::registerTeam(const std::string& teamID, const std::string& regType) {
    if (registeredCount >= MAX_CAPACITY) {
        std::cout << "Tournament full! Cannot register team " << teamID << std::endl;
        return false;
    }
    
    Team* team = findTeamByID(teamID);
    if (!team) {
        std::cout << "Team " << teamID << " not found!" << std::endl;
        return false;
    }
    
    // Check if already registered
    for (int i = 0; i < registeredCount; i++) {
        if (registeredTeams[i]->teamID == teamID) {
            std::cout << "Team " << teamID << " already registered!" << std::endl;
            return false;
        }
    }
    
    // Validate registration type based on team flags
    if (regType == "early-bird" && !team->isEarlyBird) {
        std::cout << "Team " << teamID << " not eligible for early-bird registration (rank > 50)!" << std::endl;
        return false;
    }
    if (regType == "wildcard" && !team->isWildcard) {
        std::cout << "Team " << teamID << " not eligible for wildcard registration (rank < 150)!" << std::endl;
        return false;
    }
    
    team->registrationType = regType;
    team->arrivalOrder = ++arrivalCounter;
    
    if (registeredCount >= registeredCapacity) {
        resizeRegisteredArray();
    }
    
    registeredTeams[registeredCount++] = team;
    insertPriorityQueue(team);
    
    // Log registration
    if (regLog) {
        std::fprintf(regLog, "%s,%s,%s,2025-06-15T00:00,REGISTERED\n",
                     team->teamID.c_str(), team->teamName.c_str(), regType.c_str());
        std::fflush(regLog);
    }
    
    std::cout << "Team " << team->teamName << " (" << regType << ") registered successfully!" << std::endl;
    return true;
}

void TournamentRegistration::processRegistrations() {
    std::cout << "\n=== Processing Registrations (Priority Order) ===" << std::endl;
    PriorityNode* current = regHead;
    int count = 1;
    
    while (current) {
        Team* team = current->team;
        std::cout << count++ << ". " << team->teamName 
                  << " (Rank: " << team->rank << ") - " 
                  << team->registrationType << " registration" << std::endl;
        current = current->next;
    }
    
    if (count == 1) {
        std::cout << "No teams registered yet." << std::endl;
    }
}

bool TournamentRegistration::checkInTeam(const std::string& teamID) {
    for (int i = 0; i < registeredCount; i++) {
        Team* team = registeredTeams[i];
        if (team->teamID == teamID && !team->checkedIn) {
            team->checkedIn = true;
            enqueueCircular(team);
            
            // Log check-in
            if (regLog) {
                std::fprintf(regLog, "%s,%s,%s,2025-06-15T00:00,CHECKED_IN\n",
                             team->teamID.c_str(), team->teamName.c_str(), 
                             team->registrationType.c_str());
                std::fflush(regLog);
            }
            
            std::cout << "Team " << team->teamName << " checked in successfully!" << std::endl;
            return true;
        }
    }
    
    std::cout << "Team " << teamID << " not found or already checked in!" << std::endl;
    return false;
}

Team* TournamentRegistration::getNextCheckedInTeam() {
    if (isCircularQueueEmpty()) {
        std::cout << "No teams in check-in queue!" << std::endl;
        return nullptr;
    }
    
    Team* team = dequeueCircular();
    std::cout << "Next team for tournament: " << team->teamName << std::endl;
    return team;
}

void TournamentRegistration::withdrawTeam(const std::string& teamID) {
    for (int i = 0; i < registeredCount; i++) {
        Team* team = registeredTeams[i];
        if (team->teamID == teamID) {
            pushWithdrawal(team);
            
            // Remove from registered teams array
            for (int j = i; j < registeredCount - 1; j++) {
                registeredTeams[j] = registeredTeams[j + 1];
            }
            registeredCount--;
            
            // Log withdrawal
            if (regLog) {
                std::fprintf(regLog, "%s,%s,%s,2025-06-15T00:00,WITHDRAWN\n",
                             team->teamID.c_str(), team->teamName.c_str(), 
                             team->registrationType.c_str());
                std::fflush(regLog);
            }
            
            std::cout << "Team " << team->teamName << " withdrawn successfully!" << std::endl;
            return;
        }
    }
    std::cout << "Team " << teamID << " not found!" << std::endl;
}

void TournamentRegistration::addReplacementTeam(const std::string& teamID) {
    if (registeredCount >= MAX_CAPACITY) {
        std::cout << "Tournament full! Cannot add replacement team." << std::endl;
        return;
    }
    
    Team* team = findTeamByID(teamID);
    if (!team) {
        std::cout << "Team " << teamID << " not found!" << std::endl;
        return;
    }
    
    // Default to regular registration for replacements
    team->registrationType = "regular";
    team->arrivalOrder = ++arrivalCounter;
    
    if (registeredCount >= registeredCapacity) {
        resizeRegisteredArray();
    }
    
    registeredTeams[registeredCount++] = team;
    insertPriorityQueue(team);
    
    std::cout << "Replacement team " << team->teamName << " added successfully!" << std::endl;
}

Team* TournamentRegistration::popWithdrawal() {
    if (isStackEmpty()) {
        std::cout << "No withdrawn teams!" << std::endl;
        return nullptr;
    }
    
    StackNode* temp = withdrawalTop;
    Team* team = temp->team;
    withdrawalTop = withdrawalTop->next;
    delete temp;
    
    return team;
}

void TournamentRegistration::displayWithdrawalStack() {
    std::cout << "\n=== Withdrawn Teams (Stack Order) ===" << std::endl;
    StackNode* current = withdrawalTop;
    int count = 1;
    
    while (current) {
        std::cout << count++ << ". " << current->team->teamName 
                  << " (Rank: " << current->team->rank << ")" << std::endl;
        current = current->next;
    }
    
    if (count == 1) {
        std::cout << "No withdrawn teams." << std::endl;
    }
}

void TournamentRegistration::displayStats() {
    std::cout << "\n=== Tournament Registration Statistics ===" << std::endl;
    std::cout << "Total Registered: " << registeredCount << "/" << MAX_CAPACITY << std::endl;
    
    int checkedIn = 0;
    int earlyBird = 0, regular = 0, wildcard = 0;
    
    for (int i = 0; i < registeredCount; i++) {
        Team* team = registeredTeams[i];
        if (team->checkedIn) checkedIn++;
        
        if (team->registrationType == "early-bird") earlyBird++;
        else if (team->registrationType == "regular") regular++;
        else if (team->registrationType == "wildcard") wildcard++;
    }
    
    std::cout << "Checked In: " << checkedIn << std::endl;
    std::cout << "In Check-in Queue: " << queueCount << std::endl;
    
    std::cout << "\nRegistration Types:" << std::endl;
    std::cout << "- Early Bird: " << earlyBird << std::endl;
    std::cout << "- Regular: " << regular << std::endl;
    std::cout << "- Wildcard: " << wildcard << std::endl;
}

void TournamentRegistration::displayCheckInQueue() {
    std::cout << "\n=== Teams Checked In (Circular Queue) ===" << std::endl;
    
    if (isCircularQueueEmpty()) {
        std::cout << "No teams checked in yet." << std::endl;
        return;
    }
    
    int current = front;
    int count = 1;
    
    for (int i = 0; i < queueCount; i++) {
        Team* team = circularQueue[current].team;
        if (team) {
            std::cout << count++ << ". " << team->teamName 
                      << " (Rank: " << team->rank << ")" << std::endl;
        }
        current = circularQueue[current].next;
    }
}

void TournamentRegistration::displayMenu() {
    std::cout << "\n=== Tournament Registration Menu ===" << std::endl;
    std::cout << "1. Register Team (Early-bird)" << std::endl;
    std::cout << "2. Register Team (Regular)" << std::endl;
    std::cout << "3. Register Team (Wildcard)" << std::endl;
    std::cout << "4. Check-in Team" << std::endl;
    std::cout << "5. Withdraw Team" << std::endl;
    std::cout << "6. Add Replacement Team" << std::endl;
    std::cout << "7. Process Next Check-in" << std::endl;
    std::cout << "8. View Registration Queue" << std::endl;
    std::cout << "9. View Check-in Queue" << std::endl;
    std::cout << "10. View Withdrawal Stack" << std::endl;
    std::cout << "11. View Statistics" << std::endl;
    std::cout << "0. Return to Main Menu" << std::endl;
}

void TournamentRegistration::handleUserInput(int choice) {
    std::string teamID;
    
    switch (choice) {
        case 1:
            std::cout << "Enter Team ID for early-bird registration: ";
            std::cin >> teamID;
            registerTeam(teamID, "early-bird");
            break;
        case 2:
            std::cout << "Enter Team ID for regular registration: ";
            std::cin >> teamID;
            registerTeam(teamID, "regular");
            break;
        case 3:
            std::cout << "Enter Team ID for wildcard registration: ";
            std::cin >> teamID;
            registerTeam(teamID, "wildcard");
            break;
        case 4:
            std::cout << "Enter Team ID to check-in: ";
            std::cin >> teamID;
            checkInTeam(teamID);
            break;
        case 5:
            std::cout << "Enter Team ID to withdraw: ";
            std::cin >> teamID;
            withdrawTeam(teamID);
            break;
        case 6:
            std::cout << "Enter Team ID for replacement: ";
            std::cin >> teamID;
            addReplacementTeam(teamID);
            break;
        case 7:
            getNextCheckedInTeam();
            break;
        case 8:
            processRegistrations();
            break;
        case 9:
            displayCheckInQueue();
            break;
        case 10:
            displayWithdrawalStack();
            break;
        case 11:
            displayStats();
            break;
        default:
            std::cout << "Invalid choice!" << std::endl;
    }
}

// Private helper functions
void TournamentRegistration::insertPriorityQueue(Team* team) {
    PriorityNode* newNode = new PriorityNode(team);
    int teamPriority = getPriority(team->registrationType);
    
    if (!regHead || getPriority(regHead->team->registrationType) > teamPriority ||
        (getPriority(regHead->team->registrationType) == teamPriority && 
         regHead->team->arrivalOrder > team->arrivalOrder)) {
        newNode->next = regHead;
        regHead = newNode;
        return;
    }
    
    PriorityNode* current = regHead;
    while (current->next && 
           (getPriority(current->next->team->registrationType) < teamPriority ||
            (getPriority(current->next->team->registrationType) == teamPriority &&
             current->next->team->arrivalOrder < team->arrivalOrder))) {
        current = current->next;
    }
    
    newNode->next = current->next;
    current->next = newNode;
}

// Circular Queue operations
bool TournamentRegistration::isCircularQueueFull() {
    return queueCount >= CIRCULAR_SIZE;
}

bool TournamentRegistration::isCircularQueueEmpty() {
    return queueCount == 0;
}

void TournamentRegistration::enqueueCircular(Team* team) {
    if (isCircularQueueFull()) {
        std::cout << "Check-in queue is full!" << std::endl;
        return;
    }
    
    if (isCircularQueueEmpty()) {
        front = rear = 0;
    } else {
        rear = circularQueue[rear].next;
    }
    
    circularQueue[rear].team = team;
    queueCount++;
}

Team* TournamentRegistration::dequeueCircular() {
    if (isCircularQueueEmpty()) {
        return nullptr;
    }
    
    Team* team = circularQueue[front].team;
    circularQueue[front].team = nullptr;
    
    if (queueCount == 1) {
        front = rear = -1;
    } else {
        front = circularQueue[front].next;
    }
    
    queueCount--;
    return team;
}

// Stack operations
void TournamentRegistration::pushWithdrawal(Team* team) {
    StackNode* newNode = new StackNode(team);
    newNode->next = withdrawalTop;
    withdrawalTop = newNode;
}

bool TournamentRegistration::isStackEmpty() {
    return withdrawalTop == nullptr;
}

Team* TournamentRegistration::findTeamByID(const std::string& teamID) {
    for (int i = 0; i < teamCount; i++) {
        if (allTeams[i].teamID == teamID) {
            return &allTeams[i];
        }
    }
    return nullptr;
}

int TournamentRegistration::getPriority(const std::string& regType) {
    if (regType == "early-bird") return 1;
    if (regType == "wildcard") return 2;
    return 3; // regular
}

void TournamentRegistration::parseLine(const std::string& line, std::string* fields, int expectedFields) {
    std::stringstream ss(line);
    std::string item;
    int idx = 0;
    while (std::getline(ss, item, ',') && idx < expectedFields) {
        fields[idx++] = item;
    }
}

void TournamentRegistration::resizeTeamArray() {
    int newCapacity = teamCapacity * 2;
    Team* newArray = new Team[newCapacity];
    for (int i = 0; i < teamCount; i++) {
        newArray[i] = allTeams[i];
    }
    delete[] allTeams;
    allTeams = newArray;
    teamCapacity = newCapacity;
}

void TournamentRegistration::resizeRegisteredArray() {
    int newCapacity = registeredCapacity * 2;
    Team** newArray = new Team*[newCapacity];
    for (int i = 0; i < registeredCount; i++) {
        newArray[i] = registeredTeams[i];
    }
    delete[] registeredTeams;
    registeredTeams = newArray;
    registeredCapacity = newCapacity;
}

void TournamentRegistration::clearPriorityQueue() {
    while (regHead) {
        PriorityNode* temp = regHead;
        regHead = regHead->next;
        delete temp;
    }
}

void TournamentRegistration::clearWithdrawalStack() {
    while (withdrawalTop) {
        StackNode* temp = withdrawalTop;
        withdrawalTop = withdrawalTop->next;
        delete temp;
    }
}

bool TournamentRegistration::isEmpty() const {
    return registeredCount == 0;
}

int TournamentRegistration::getRegisteredCount() const {
    return registeredCount;
}
