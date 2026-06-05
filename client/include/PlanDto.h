#pragma once
#include <string>
#include <vector>

struct PlanDto {
    std::string rawResponse;
    std::vector<std::string> plans;
};