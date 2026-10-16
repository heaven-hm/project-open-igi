#include "igi/mission_objective_loader.hpp"

#include <charconv>
#include <cstddef>
#include <string_view>
#include <utility>

namespace igi::gameplay {

namespace {

constexpr std::string_view kDefineComputerObjective = "DefineComputerObjective";
constexpr std::size_t kMinimumTaskArgumentCount = 4;
constexpr std::size_t kFirstObjectiveArgumentIndex = 4;
constexpr std::size_t kObjectiveSlotArgumentCount = 4;
constexpr std::size_t kObjectiveSlotCount = 6;

std::string unquote_token(std::string token) {
    if (token.size() < 2 || token.front() != '"' || token.back() != '"') {
        return token;
    }

    token = token.substr(1, token.size() - 2);
    std::string unquoted;
    unquoted.reserve(token.size());
    bool escaped = false;
    for (const char c : token) {
        if (escaped) {
            unquoted.push_back(c);
            escaped = false;
        } else if (c == '\\') {
            escaped = true;
        } else {
            unquoted.push_back(c);
        }
    }
    if (escaped) {
        unquoted.push_back('\\');
    }
    return unquoted;
}

bool try_parse_integer(const std::string& token, std::int32_t& value) {
    const std::string unquoted = unquote_token(token);
    const char* begin = unquoted.data();
    const char* end = begin + unquoted.size();
    const auto result = std::from_chars(begin, end, value);
    return result.ec == std::errc() && result.ptr == end;
}

} // namespace

std::vector<AuthoredMissionObjectiveSet> load_authored_mission_objective_definitions(
    const std::vector<MissionObjectiveTaskSource>& task_sources) {
    std::vector<AuthoredMissionObjectiveSet> definitions;

    for (const MissionObjectiveTaskSource& task_source : task_sources) {
        if (task_source.task_type != kDefineComputerObjective ||
            task_source.argument_tokens.size() < kMinimumTaskArgumentCount) {
            continue;
        }

        AuthoredMissionObjectiveSet objective_set;
        objective_set.valid_expression = unquote_token(task_source.argument_tokens[3]);

        for (std::size_t slot_index = 0; slot_index < kObjectiveSlotCount; ++slot_index) {
            const std::size_t argument_index =
                kFirstObjectiveArgumentIndex + slot_index * kObjectiveSlotArgumentCount;
            if (argument_index >= task_source.argument_tokens.size()) {
                break;
            }

            const std::string text_resource =
                unquote_token(task_source.argument_tokens[argument_index]);
            if (text_resource.empty()) {
                continue;
            }

            AuthoredMissionObjectiveDefinition objective;
            objective.text_resource = text_resource;

            if (argument_index + 1 < task_source.argument_tokens.size()) {
                try_parse_integer(
                    task_source.argument_tokens[argument_index + 1],
                    objective.link_task_id);
            }
            if (argument_index + 2 < task_source.argument_tokens.size()) {
                objective.completion_expression = unquote_token(
                    task_source.argument_tokens[argument_index + 2]);
            }
            if (argument_index + 3 < task_source.argument_tokens.size()) {
                objective.failure_expression = unquote_token(
                    task_source.argument_tokens[argument_index + 3]);
            }
            objective_set.objectives.push_back(std::move(objective));
        }

        if (!objective_set.objectives.empty()) {
            definitions.push_back(std::move(objective_set));
        }
    }

    return definitions;
}

} // namespace igi::gameplay
