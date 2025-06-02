#pragma once
#include <string>

struct Team {
    std::string teamID;
    std::string teamName;
    int rank;
    std::string registrationType; // "early-bird", "regular", "wildcard"
    bool checkedIn;
    bool isEarlyBird;    // Flag for early-bird teams (rank <= 50)
    bool isWildcard;     // Flag for wildcard teams (rank >= 150)
    int arrivalOrder;    // For tie-breaking in queues
    
    Team() : rank(0), checkedIn(false), isEarlyBird(false), isWildcard(false), arrivalOrder(0) {}
};
