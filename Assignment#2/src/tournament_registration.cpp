#include "tournament_registration.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

TournamentRegistration::TournamentRegistration() 
    : regHead(nullptr), checkInFront(nullptr), checkInRear(nullptr),
      withdrawalFront(nullptr), withdrawalRear(nullptr),
      allTeams(nullptr), teamCount(0), teamCapacity(10),
      registeredTeams(nullptr), registeredCount(0), registeredCapacity(10),
      arrivalCounter(0)
{
    allTeams = new Team[teamCapacity];
    registeredTeams = new Team*[registeredCapacity];
}

TournamentRegistration::~TournamentRegistration() {
    clearPriorityQueue();
    clearQueue(checkInFront, checkInRear);
    clearQueue(withdrawalFront, withdrawalRear);
    delete[] allTeams;
    delete[] registeredTeams;
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
        teamCount++;
    }
    file.close();
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
    
    team->registrationType = regType;
    team->arrivalOrder = ++arrivalCounter;
    
    if (registeredCount >= registeredCapacity) {
        resizeRegisteredArray();
    }
    
    registeredTeams[registeredCount++] = team;
    insertPriorityQueue(team);
    
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
            enqueueRegular(checkInFront, checkInRear, team);
            std::cout << "Team " << team->teamName << " checked in successfully!" << std::endl;
            return true;
        }
    }
    
    std::cout << "Team " << teamID << " not found or already checked in!" << std::endl;
    return false;
}

void TournamentRegistration::withdrawTeam(const std::string& teamID) {
    for (int i = 0; i < registeredCount; i++) {
        Team* team = registeredTeams[i];
        if (team->teamID == teamID) {
            enqueueRegular(withdrawalFront, withdrawalRear, team);
            // Remove from registered teams array
            for (int j = i; j < registeredCount - 1; j++) {
                registeredTeams[j] = registeredTeams[j + 1];
            }
            registeredCount--;
            std::cout << "Team " << team->teamName << " withdrawn successfully!" << std::endl;
            return;
        }
    }
    std::cout << "Team " << teamID << " not found!" << std::endl;
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
    std::cout << "Pending Check-in: " << (registeredCount - checkedIn) << std::endl;
    
    std::cout << "\nRegistration Types:" << std::endl;
    std::cout << "- Early Bird: " << earlyBird << std::endl;
    std::cout << "- Regular: " << regular << std::endl;
    std::cout << "- Wildcard: " << wildcard << std::endl;
}

void TournamentRegistration::displayCheckInQueue() {
    std::cout << "\n=== Teams Checked In (FIFO Order) ===" << std::endl;
    QueueNode* current = checkInFront;
    int count = 1;
    
    while (current) {
        Team* team = current->team;
        std::cout << count++ << ". " << team->teamName 
                  << " (Rank: " << team->rank << ")" << std::endl;
        current = current->next;
    }
    
    if (count == 1) {
        std::cout << "No teams checked in yet." << std::endl;
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

void TournamentRegistration::enqueueRegular(QueueNode*& front, QueueNode*& rear, Team* team) {
    QueueNode* newNode = new QueueNode(team);
    if (!rear) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
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

void TournamentRegistration::clearQueue(QueueNode*& front, QueueNode*& rear) {
    while (front) {
        QueueNode* temp = front;
        front = front->next;
        delete temp;
    }
    rear = nullptr;
}

bool TournamentRegistration::isEmpty() const {
    return registeredCount == 0;
}

int TournamentRegistration::getRegisteredCount() const {
    return registeredCount;
}
