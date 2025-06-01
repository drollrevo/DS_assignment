#pragma once
#include <string>

struct Team {
    std::string teamID;
    std::string teamName;
    int rank;
    std::string registrationType; // "early-bird", "regular", "wildcard"
    bool checkedIn;
    int arrivalOrder; // For tie-breaking in queues
    
    Team() : rank(0), checkedIn(false), arrivalOrder(0) {}
};
