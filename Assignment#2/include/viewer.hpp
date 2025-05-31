#pragma once
#include <string>

struct Viewer {
    std::string id;
    std::string name;
    int priority;       // 1: VIP, 2: Influencer, 3: Usual
    int arrivalOrder;   // For tie-breaking: lower = earlier
};
