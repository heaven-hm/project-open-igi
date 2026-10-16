#pragma once

#include "igi/level_flow.hpp"

#include <string>
#include <vector>

namespace igi::gameplay {

struct MissionObjectiveTaskSource {
    std::string task_type;
    std::vector<std::string> argument_tokens;
};

// Reads the six fixed DefineComputerObjective slots used by the vanilla QVM / QSC scripts.
std::vector<AuthoredMissionObjectiveSet> load_authored_mission_objective_definitions(
    const std::vector<MissionObjectiveTaskSource>& task_sources);

} // namespace igi::gameplay
