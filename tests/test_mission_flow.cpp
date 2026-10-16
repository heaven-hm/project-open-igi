#include "test_framework.hpp"
#include "igi/level_flow.hpp"
#include "igi/mission_expression.hpp"
#include "igi/mission_objective_loader.hpp"

using namespace igi::gameplay;

TEST_CASE(MissionFlow, MissionExpressionArithmeticAndLogic) {
    MissionExpressionState state;
    state.set_boolean("ALARM_TRIGGERED", false);
    state.set_boolean("RADIO_DISABLED", true);
    state.set_number("SOLDIER_COUNT", 5.0);

    bool bool_val = false;
    double num_val = 0.0;

    // Direct literals
    ASSERT_TRUE(state.try_evaluate_boolean("TRUE", bool_val));
    ASSERT_TRUE(bool_val);
    ASSERT_TRUE(state.try_evaluate_boolean("FALSE", bool_val));
    ASSERT_FALSE(bool_val);

    // GAME_FREQUENCY constant check
    ASSERT_TRUE(state.try_evaluate_number("GAME_FREQUENCY", num_val));
    ASSERT_EQ(num_val, 30.0);

    // Logical AND / OR / NOT
    ASSERT_TRUE(state.try_evaluate_boolean("RADIO_DISABLED && !ALARM_TRIGGERED", bool_val));
    ASSERT_TRUE(bool_val);
    ASSERT_TRUE(state.try_evaluate_boolean("ALARM_TRIGGERED || RADIO_DISABLED", bool_val));
    ASSERT_TRUE(bool_val);
    ASSERT_TRUE(state.try_evaluate_boolean("ALARM_TRIGGERED && RADIO_DISABLED", bool_val));
    ASSERT_FALSE(bool_val);

    // Relational comparisons
    ASSERT_TRUE(state.try_evaluate_boolean("SOLDIER_COUNT > 3", bool_val));
    ASSERT_TRUE(bool_val);
    ASSERT_TRUE(state.try_evaluate_boolean("SOLDIER_COUNT <= 5", bool_val));
    ASSERT_TRUE(bool_val);
    ASSERT_TRUE(state.try_evaluate_boolean("SOLDIER_COUNT == 5", bool_val));
    ASSERT_TRUE(bool_val);
    ASSERT_TRUE(state.try_evaluate_boolean("SOLDIER_COUNT != 5", bool_val));
    ASSERT_FALSE(bool_val);

    // Arithmetic
    ASSERT_TRUE(state.try_evaluate_number("SOLDIER_COUNT * 2 + 10 / 2", num_val));
    ASSERT_EQ(num_val, 15.0);

    // Parentheses grouping
    ASSERT_TRUE(state.try_evaluate_number("(SOLDIER_COUNT + 5) * 3", num_val));
    ASSERT_EQ(num_val, 30.0);

    // Division by zero returns false
    ASSERT_FALSE(state.try_evaluate_number("SOLDIER_COUNT / 0", num_val));

    // Unknown variable returns false
    ASSERT_FALSE(state.try_evaluate_boolean("UNKNOWN_VAR == 1", bool_val));
}

TEST_CASE(MissionFlow, LevelFlowFallbackMissions) {
    LevelFlow flow;

    // Mission 1 initialization
    flow.initialize_mission(1);
    ASSERT_EQ(flow.mission_number(), 1u);
    ASSERT_TRUE(flow.status() == MissionStatus::InProgress);
    ASSERT_EQ(flow.objectives().size(), 2u);
    ASSERT_TRUE(flow.objectives()[0].is_primary);
    ASSERT_TRUE(flow.objectives()[0].state == ObjectiveState::Pending);
    ASSERT_NE(flow.get_objective_display_text(), "");

    // Completing first objective
    ASSERT_TRUE(flow.complete_first_pending_primary_objective());
    ASSERT_TRUE(flow.objectives()[0].state == ObjectiveState::Completed);

    // All 14 missions have fallback definitions
    for (std::uint32_t m = 1; m <= 14; ++m) {
        LevelFlow mflow;
        mflow.initialize_mission(m);
        ASSERT_FALSE(mflow.objectives().empty());
        ASSERT_TRUE(mflow.status() == MissionStatus::InProgress);
    }
}

TEST_CASE(MissionFlow, LevelFlowStateProgressionAndExpressions) {
    LevelFlow flow;
    flow.initialize_mission(1);

    MissionExpressionState state;
    state.set_boolean("TRAIN_DOWNLOADED", false);
    state.set_boolean("SECURITY_ALERTED", false);

    const auto eval = [&state](const std::string& expr) -> bool {
        bool result = false;
        return state.try_evaluate_boolean(expr, result) && result;
    };

    // Player dead triggers mission failure
    flow.update(false, false, eval);
    ASSERT_TRUE(flow.status() == MissionStatus::Failed);
    ASSERT_EQ(flow.get_objective_display_text(), "MISSION FAILED");

    // Re-initialize mission
    flow.initialize_mission(1);
    ASSERT_TRUE(flow.status() == MissionStatus::InProgress);

    // Complete objectives manually and extract
    flow.complete_first_pending_primary_objective();
    flow.update(true, true, eval);
    ASSERT_TRUE(flow.status() == MissionStatus::Success);
    ASSERT_EQ(flow.get_objective_display_text(), "MISSION COMPLETE");
}

TEST_CASE(MissionFlow, MissionObjectiveLoaderParsing) {
    // Construct synthetic DefineComputerObjective tasks matching vanilla QSC
    MissionObjectiveTaskSource task1;
    task1.task_type = "DefineComputerObjective";
    task1.argument_tokens = {
        "1", "0", "0", "\"TRUE\"",
        "\"obj_infiltrate\"", "4001", "\"DOWNLOAD_DONE == 1\"", "\"\"",
        "\"obj_escape\"", "4002", "\"AT_EXTRACTION == 1\"", "\"TIME_UP == 1\"",
    };

    const std::vector<AuthoredMissionObjectiveSet> sets =
        load_authored_mission_objective_definitions({task1});

    ASSERT_EQ(sets.size(), 1u);
    ASSERT_EQ(sets[0].valid_expression, "TRUE");
    ASSERT_EQ(sets[0].objectives.size(), 2u);

    ASSERT_EQ(sets[0].objectives[0].text_resource, "obj_infiltrate");
    ASSERT_EQ(sets[0].objectives[0].link_task_id, 4001);
    ASSERT_EQ(sets[0].objectives[0].completion_expression, "DOWNLOAD_DONE == 1");
    ASSERT_EQ(sets[0].objectives[0].failure_expression, "");

    ASSERT_EQ(sets[0].objectives[1].text_resource, "obj_escape");
    ASSERT_EQ(sets[0].objectives[1].link_task_id, 4002);
    ASSERT_EQ(sets[0].objectives[1].completion_expression, "AT_EXTRACTION == 1");
    ASSERT_EQ(sets[0].objectives[1].failure_expression, "TIME_UP == 1");
}
