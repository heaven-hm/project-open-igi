#include "igi/level_flow.hpp"
#include "igi/logger.hpp"

#include <utility>

namespace igi::gameplay {

LevelFlow::LevelFlow() {
    initialize_mission(1);
}

void LevelFlow::initialize_mission(std::uint32_t mission_number) {
    mission_number_ = mission_number;
    objectives_.clear();
    status_ = MissionStatus::InProgress;
    authored_objective_sets_.clear();
    objective_text_resolver_ = {};
    objective_location_resolver_ = {};
    active_authored_objective_set_index_ = 0;
    authored_mission_flow_ = {};
    has_authored_mission_flow_ = false;
    mission_flow_tick_ = 0;

    initialize_fallback_objectives(mission_number);
    IGI_LOG_INFO("LevelFlow", "Initialized mission " << mission_number << " with " << objectives_.size() << " fallback objectives");
}

void LevelFlow::initialize_mission(
    std::uint32_t mission_number,
    const std::vector<AuthoredMissionObjectiveSet>& authored_objective_sets,
    MissionObjectiveTextResolver text_resolver,
    const AuthoredMissionFlowDefinition& authored_flow,
    MissionObjectiveLocationResolver location_resolver) {
    mission_number_ = mission_number;
    objectives_.clear();
    status_ = MissionStatus::InProgress;
    authored_objective_sets_ = authored_objective_sets;
    objective_text_resolver_ = std::move(text_resolver);
    objective_location_resolver_ = std::move(location_resolver);
    active_authored_objective_set_index_ = 0;
    authored_mission_flow_ = authored_flow;
    has_authored_mission_flow_ = authored_flow.has_level_flow ||
        !authored_flow.complete_expression.empty() ||
        !authored_flow.failure_expression.empty();
    mission_flow_tick_ = 0;

    if (authored_objective_sets_.empty()) {
        initialize_fallback_objectives(mission_number);
        return;
    }

    load_authored_objective_set(active_authored_objective_set_index_);
    if (objectives_.empty()) {
        authored_objective_sets_.clear();
        initialize_fallback_objectives(mission_number);
    }
    IGI_LOG_INFO("LevelFlow", "Initialized mission " << mission_number << " with " << objectives_.size() << " authored objectives");
}

void LevelFlow::initialize_fallback_objectives(std::uint32_t mission_number) {
    switch (mission_number) {
        case 1:
            add_objective(1, "Infiltrate the trainyard and download train schedule", true);
            add_objective(2, "Get to the train depot and rendezvous with Anya", false);
            break;
        case 2:
            add_objective(1, "Infiltrate the SAM base and disable the SAM sites", true);
            add_objective(2, "Locate Josef Priboi and extract via helicopter", false);
            break;
        case 3:
            add_objective(1, "Infiltrate the military airbase without raising alarms", true);
            add_objective(2, "Locate and steal the MiG-29 fighter jet", false);
            break;
        case 4:
            add_objective(1, "Survive the plane crash and evade pursuit patrols", true);
            add_objective(2, "Reach the hilltop extraction helicopter", false);
            break;
        case 5:
            add_objective(1, "Infiltrate the radar base and sabotage the dish", true);
            add_objective(2, "Destroy communication arrays and reach the exit", false);
            break;
        case 6:
            add_objective(1, "Locate Josef Priboi in the high-security bunker", true);
            add_objective(2, "Capture Josef Priboi alive and escort to extraction", false);
            break;
        case 7:
            add_objective(1, "Cross the heavily fortified border zone", true);
            add_objective(2, "Avoid searchlights and border patrol towers", false);
            break;
        case 8:
            add_objective(1, "Infiltrate the weapons facility and recover gear", true);
            add_objective(2, "Sabotage heavy armory and escape undetected", false);
            break;
        case 9:
            add_objective(1, "Locate the nuclear missile transport train", true);
            add_objective(2, "Disable missile guidance computers on the train", false);
            break;
        case 10:
            add_objective(1, "Defend Josef Priboi from enemy assault waves", true);
            add_objective(2, "Eliminate all incoming enemy soldiers and snipers", false);
            break;
        case 11:
            add_objective(1, "Infiltrate Jach Priboi's mountain castle fortress", true);
            add_objective(2, "Disable exterior perimeter security systems", false);
            break;
        case 12:
            add_objective(1, "Breach the inner sanctum and locate Jach Priboi", true);
            add_objective(2, "Apprehend Jach Priboi and clear the helicopter LZ", false);
            break;
        case 13:
            add_objective(1, "Infiltrate the underground nuclear warhead complex", true);
            add_objective(2, "Locate the primary reactor core and disable cooling", false);
            break;
        case 14:
            add_objective(1, "Track down Ekk in the nuclear launch silo", true);
            add_objective(2, "Eliminate Ekk and disarm the nuclear bomb", false);
            break;
        default:
            add_objective(1, "Complete mission objectives and extract safely", true);
            break;
    }
}

void LevelFlow::add_objective(std::uint32_t id, const std::string& desc, bool is_primary) {
    MissionObjective obj;
    obj.id = id;
    obj.description = desc;
    obj.is_primary = is_primary;
    obj.state = ObjectiveState::Pending;
    objectives_.push_back(std::move(obj));
}

void LevelFlow::load_authored_objective_set(std::size_t authored_set_index) {
    objectives_.clear();
    if (authored_set_index >= authored_objective_sets_.size()) {
        return;
    }

    const AuthoredMissionObjectiveSet& objective_set = authored_objective_sets_[authored_set_index];
    for (std::size_t objective_index = 0; objective_index < objective_set.objectives.size(); ++objective_index) {
        const AuthoredMissionObjectiveDefinition& authored_objective = objective_set.objectives[objective_index];
        std::string display_text = authored_objective.text_resource;
        if (objective_text_resolver_) {
            const std::string resolved = objective_text_resolver_(authored_objective.text_resource);
            if (!resolved.empty()) {
                display_text = resolved;
            }
        }

        MissionObjective obj;
        obj.id = static_cast<std::uint32_t>(objective_index + 1);
        obj.description = std::move(display_text);
        obj.text_resource = authored_objective.text_resource;
        obj.link_task_id = authored_objective.link_task_id;
        obj.completion_expression = authored_objective.completion_expression;
        obj.failure_expression = authored_objective.failure_expression;
        obj.is_primary = true;
        obj.state = ObjectiveState::Pending;
        if (obj.link_task_id >= 0 && objective_location_resolver_) {
            MissionObjectiveLocation loc;
            if (objective_location_resolver_(obj.link_task_id, loc)) {
                obj.has_location = true;
                obj.location = loc;
            }
        }
        objectives_.push_back(std::move(obj));
    }
}

bool LevelFlow::advance_authored_objective_set() {
    if (authored_objective_sets_.empty() ||
        active_authored_objective_set_index_ + 1 >= authored_objective_sets_.size()) {
        return false;
    }
    ++active_authored_objective_set_index_;
    load_authored_objective_set(active_authored_objective_set_index_);
    return !objectives_.empty();
}

void LevelFlow::select_valid_authored_objective_set(const MissionExpressionEvaluator& expression_evaluator) {
    if (authored_objective_sets_.empty() || !expression_evaluator) {
        return;
    }

    std::size_t selected_definition_index = 0;
    bool has_valid_definition = false;
    for (std::size_t definition_index = 0; definition_index < authored_objective_sets_.size(); ++definition_index) {
        const AuthoredMissionObjectiveSet& definition = authored_objective_sets_[definition_index];
        const bool is_valid = definition.valid_expression.empty() ||
            expression_evaluator(definition.valid_expression);
        if (!is_valid) {
            continue;
        }
        selected_definition_index = definition_index;
        has_valid_definition = true;
    }

    if (!has_valid_definition || selected_definition_index == active_authored_objective_set_index_) {
        return;
    }

    active_authored_objective_set_index_ = selected_definition_index;
    load_authored_objective_set(active_authored_objective_set_index_);
}

void LevelFlow::set_objective_state(std::uint32_t id, ObjectiveState state) {
    bool objective_was_updated = false;
    for (MissionObjective& obj : objectives_) {
        if (obj.id == id) {
            obj.state = state;
            objective_was_updated = true;
            break;
        }
    }

    if (!objective_was_updated || state != ObjectiveState::Completed) {
        return;
    }

    for (const MissionObjective& obj : objectives_) {
        if (obj.is_primary && obj.state == ObjectiveState::Pending) {
            return;
        }
    }
    advance_authored_objective_set();
}

bool LevelFlow::complete_first_pending_primary_objective() {
    if (status_ != MissionStatus::InProgress) {
        return false;
    }

    bool objective_was_completed = false;
    for (MissionObjective& obj : objectives_) {
        if (!obj.is_primary || obj.state != ObjectiveState::Pending) {
            continue;
        }
        obj.state = ObjectiveState::Completed;
        objective_was_completed = true;
        break;
    }

    if (!objective_was_completed) {
        return false;
    }

    for (const MissionObjective& obj : objectives_) {
        if (obj.is_primary && obj.state == ObjectiveState::Pending) {
            return true;
        }
    }
    advance_authored_objective_set();
    return true;
}

std::string LevelFlow::get_objective_display_text() const {
    if (status_ == MissionStatus::Failed) {
        return "MISSION FAILED";
    }
    if (status_ == MissionStatus::Success) {
        return "MISSION COMPLETE";
    }

    for (const auto& obj : objectives_) {
        if (obj.is_primary && obj.state == ObjectiveState::Pending) {
            return obj.description;
        }
    }
    return "Reach the extraction zone";
}

void LevelFlow::evaluate_authored_objective_expressions(const MissionExpressionEvaluator& expression_evaluator) {
    if (!expression_evaluator || authored_objective_sets_.empty()) {
        return;
    }

    bool objective_failed = false;
    for (MissionObjective& obj : objectives_) {
        if (obj.state != ObjectiveState::Pending) {
            continue;
        }

        if (!obj.failure_expression.empty() && expression_evaluator(obj.failure_expression)) {
            obj.state = ObjectiveState::Failed;
            objective_failed = true;
            continue;
        }

        if (!obj.completion_expression.empty() && expression_evaluator(obj.completion_expression)) {
            obj.state = ObjectiveState::Completed;
        }
    }

    if (objective_failed) {
        return;
    }

    for (const MissionObjective& obj : objectives_) {
        if (obj.is_primary && obj.state == ObjectiveState::Pending) {
            return;
        }
    }
    advance_authored_objective_set();
}

bool LevelFlow::evaluate_expression(
    const std::string& expression,
    const MissionExpressionEvaluator& expression_evaluator) const {
    return !expression.empty() && expression_evaluator && expression_evaluator(expression);
}

bool LevelFlow::evaluate_authored_mission_flow(const MissionExpressionEvaluator& expression_evaluator) {
    if (!has_authored_mission_flow_) {
        return false;
    }

    if (evaluate_expression(authored_mission_flow_.failure_expression, expression_evaluator)) {
        status_ = MissionStatus::Failed;
        return true;
    }
    if (evaluate_expression(authored_mission_flow_.complete_expression, expression_evaluator)) {
        status_ = MissionStatus::Success;
        return true;
    }
    return false;
}

void LevelFlow::update(
    bool player_alive,
    bool in_extraction_zone,
    MissionExpressionEvaluator expression_evaluator) {
    if (status_ != MissionStatus::InProgress) {
        return;
    }

    if (!player_alive) {
        status_ = MissionStatus::Failed;
        return;
    }

    select_valid_authored_objective_set(expression_evaluator);
    evaluate_authored_objective_expressions(expression_evaluator);

    bool all_primaries_complete = true;
    for (const auto& obj : objectives_) {
        if (obj.is_primary) {
            if (obj.state == ObjectiveState::Failed) {
                status_ = MissionStatus::Failed;
                return;
            }
            if (obj.state != ObjectiveState::Completed) {
                all_primaries_complete = false;
            }
        }
    }

    if (has_authored_mission_flow_) {
        evaluate_authored_mission_flow(expression_evaluator);
        ++mission_flow_tick_;
        return;
    }

    if (all_primaries_complete && in_extraction_zone) {
        status_ = MissionStatus::Success;
    }

    ++mission_flow_tick_;
}

} // namespace igi::gameplay
