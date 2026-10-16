#pragma once

#include "igi/mission_expression.hpp"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace igi::gameplay {

enum class ObjectiveState {
    Pending,
    Completed,
    Failed,
    Cancelled
};

enum class MissionStatus {
    InProgress,
    Success,
    Failed
};

struct MissionObjectiveLocation {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
};

struct MissionObjective {
    std::uint32_t id = 0;
    std::string description;
    std::string text_resource;
    std::int32_t link_task_id = -1;
    std::string completion_expression;
    std::string failure_expression;
    bool is_primary = true;
    ObjectiveState state = ObjectiveState::Pending;
    bool has_location = false;
    MissionObjectiveLocation location{};
};

struct AuthoredMissionObjectiveDefinition {
    std::string text_resource;
    std::int32_t link_task_id = -1;
    std::string completion_expression;
    std::string failure_expression;
};

struct AuthoredMissionObjectiveSet {
    std::string valid_expression;
    std::vector<AuthoredMissionObjectiveDefinition> objectives;
};

struct AuthoredMissionFlowDefinition {
    double start_time_seconds = 0.0;
    std::string complete_expression;
    std::string failure_expression;
    bool interface_timer_enabled = false;
    double maximum_level_play_time_seconds = 0.0;
    bool has_level_flow = false;
};

using MissionObjectiveTextResolver = std::function<std::string(const std::string&)>;
using MissionObjectiveLocationResolver = std::function<bool(
    std::int32_t link_task_id,
    MissionObjectiveLocation& location)>;
using MissionExpressionEvaluator = std::function<bool(const std::string&)>;

class LevelFlow final {
public:
    LevelFlow();

    void initialize_mission(std::uint32_t mission_number);
    void initialize_mission(
        std::uint32_t mission_number,
        const std::vector<AuthoredMissionObjectiveSet>& authored_objective_sets,
        MissionObjectiveTextResolver text_resolver = {},
        const AuthoredMissionFlowDefinition& authored_flow = {},
        MissionObjectiveLocationResolver location_resolver = {});

    void add_objective(std::uint32_t id, const std::string& desc, bool is_primary = true);
    void set_objective_state(std::uint32_t id, ObjectiveState state);
    bool complete_first_pending_primary_objective();

    void update(
        bool player_alive,
        bool in_extraction_zone,
        MissionExpressionEvaluator expression_evaluator = {});

    [[nodiscard]] MissionStatus status() const noexcept { return status_; }
    [[nodiscard]] const std::vector<MissionObjective>& objectives() const noexcept { return objectives_; }
    [[nodiscard]] std::uint32_t mission_number() const noexcept { return mission_number_; }
    [[nodiscard]] std::string get_objective_display_text() const;
    [[nodiscard]] bool has_authored_mission_flow() const noexcept { return has_authored_mission_flow_; }
    [[nodiscard]] std::uint64_t mission_flow_tick() const noexcept { return mission_flow_tick_; }

private:
    void initialize_fallback_objectives(std::uint32_t mission_number);
    void load_authored_objective_set(std::size_t authored_set_index);
    bool advance_authored_objective_set();
    void select_valid_authored_objective_set(const MissionExpressionEvaluator& expression_evaluator);
    void evaluate_authored_objective_expressions(const MissionExpressionEvaluator& expression_evaluator);
    bool evaluate_expression(
        const std::string& expression,
        const MissionExpressionEvaluator& expression_evaluator) const;
    bool evaluate_authored_mission_flow(const MissionExpressionEvaluator& expression_evaluator);

    std::uint32_t mission_number_ = 1;
    std::vector<MissionObjective> objectives_;
    MissionStatus status_ = MissionStatus::InProgress;
    std::vector<AuthoredMissionObjectiveSet> authored_objective_sets_;
    MissionObjectiveTextResolver objective_text_resolver_;
    MissionObjectiveLocationResolver objective_location_resolver_;
    std::size_t active_authored_objective_set_index_ = 0;
    AuthoredMissionFlowDefinition authored_mission_flow_{};
    bool has_authored_mission_flow_ = false;
    std::uint64_t mission_flow_tick_ = 0;
};

} // namespace igi::gameplay
