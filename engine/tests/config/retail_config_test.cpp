#include "igi/retail/config/retail_config.hpp"
#include "igi/retail/config/retail_input_bindings.hpp"
#include "igi/retail/input/input_port.hpp"

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace
{

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// The retail game data, when it is installed. The whole test is skipped without it.
const std::filesystem::path kGameRoot = "/Users/haseeb-mir/Downloads/IGI";

const igi::retail::config::ScriptProgram& load(const std::filesystem::path& path)
{
    static std::vector<igi::retail::config::ScriptProgram> loaded;
    loaded.emplace_back();
    std::string error;
    const bool ok = igi::retail::config::read_script_file(path, loaded.back(), error);
    require(ok, "read " + path.string() + ": " + error);
    return loaded.back();
}

const igi::retail::config::InputBinding* find_binding(
    const igi::retail::config::InputConfig& config, const std::string& action)
{
    for (const auto& binding : config.bindings) {
        if (binding.action == action) {
            return &binding;
        }
    }
    return nullptr;
}

void test_config_bindings()
{
    const auto& program = load(kGameRoot / "config.qvm");
    igi::retail::config::InputConfig config;
    std::string error;
    require(igi::retail::config::decode_input_config(program, config, error), error);

    struct Expected
    {
        const char* action;
        const char* device;
        const char* control;
    };
    const std::vector<Expected> expected{
        {"MoveLeft", "INPUTPORT_DEVICE_KEYBOARD", "KEY_LEFT"},
        {"MoveRight", "INPUTPORT_DEVICE_KEYBOARD", "KEY_RIGHT"},
        {"MoveUp", "INPUTPORT_DEVICE_KEYBOARD", "KEY_UP"},
        {"MoveDown", "INPUTPORT_DEVICE_KEYBOARD", "KEY_DOWN"},
        {"Fire", "INPUTPORT_DEVICE_MOUSE", "MOUSE_BUTTON_1"},
        {"Reload", "INPUTPORT_DEVICE_KEYBOARD", "KEY_R"},
        {"Activate", "INPUTPORT_DEVICE_KEYBOARD", "KEY_Q"},
        {"Crouch", "INPUTPORT_DEVICE_KEYBOARD", "KEY_RIGHT_CTRL"},
        {"Jump", "INPUTPORT_DEVICE_KEYBOARD", "KEY_J"},
        {"NextWeapon", "INPUTPORT_DEVICE_MOUSE", "MOUSE_BUTTON_2"},
        {"ToggleWalkRun", "INPUTPORT_DEVICE_KEYBOARD", "KEY_W"},
        {"WeaponCategory1", "INPUTPORT_DEVICE_KEYBOARD", "KEY_1"},
    };
    for (const Expected& row : expected) {
        const auto* binding = find_binding(config, row.action);
        require(binding != nullptr, std::string("missing binding ") + row.action);
        require(binding->device == row.device,
                std::string("device of ") + row.action + ": " + binding->device);
        require(binding->control == row.control,
                std::string("control of ") + row.action + ": " + binding->control);
    }
    require(config.bindings.size() == 27U,
            "shipped config.qvm binds 27 actions, got "
                + std::to_string(config.bindings.size()));

    require(config.mouse_sensitivity.has_value(), "mouse sensitivity");
    require(std::fabs(*config.mouse_sensitivity - 0.3F) < 1e-6F,
            "mouse sensitivity is the shipped 0.3");
    require(config.mouse_inverted.has_value() && *config.mouse_inverted == 0,
            "mouse inversion is the shipped 0");
    require(config.player_name.has_value() && *config.player_name == "Haseeb",
            "player name");
    require(config.active_mission.has_value() && *config.active_mission == 13,
            "active mission");
    require(config.start_state.has_value() && *config.start_state == 0, "GOStart(0)");
}

void test_human_player_general()
{
    const auto& program = load(kGameRoot / "humanplayer" / "humanplayer.qvm");
    igi::retail::config::HumanPlayerGeneral general;
    std::string error;
    require(igi::retail::config::decode_human_player_general(program, general, error), error);
    require(general.arguments.size() == 43U,
            "DefineHumanPlayerGeneral takes 43 arguments, got "
                + std::to_string(general.arguments.size()));
    require(general.root_motion_scale().value() == 1.75, "root-motion scale 1.75");
    require(general.jump_horizontal_kmh().value() == 17.5, "jump horizontal 17.5");
    require(general.jump_vertical_kmh().value() == 27.0, "jump vertical 27");
    require(general.air_control_kmh().value() == 0.5, "air control 0.5");
    require(general.arguments[4] == general.arguments[5], "the two 0.85 factors");
    require(general.arguments[42] == 100000.0, "the last argument");
}

void test_model_lod_table()
{
    const auto& program = load(kGameRoot / "LOD.QVM");
    igi::retail::config::ModelLodTable table;
    std::string error;
    require(igi::retail::config::decode_model_lod_table(program, table, error), error);
    require(table.rows.size() == 462U,
            "LOD.QVM declares 462 ModelLODSettings rows, got "
                + std::to_string(table.rows.size()));
    const auto& first = table.rows.front();
    require(first.task_name == "300_01_1", "first task name " + first.task_name);
    require(first.model == "300_01_1", "first model " + first.model);
    require(first.distance_to_lod1 == 7.0F && first.distance_to_lod2 == 17.0F
                && first.distance_to_lod3 == 30.0F && first.distance_to_lod4 == 35.0F
                && first.distance_to_cutoff == 50.0F,
            "first LOD distances");
    require(table.rows[1].task_name == "300_02_1", "second task name");
    require(table.rows[1].distance_to_lod2 == 18.0F, "second row LOD 2 distance");
}

void test_input_port_bindings()
{
    const auto& program = load(kGameRoot / "config.qvm");
    igi::retail::config::InputConfig config;
    std::string error;
    require(igi::retail::config::decode_input_config(program, config, error), error);

    igi::retail::input::InputPort port;
    require(igi::retail::config::apply_input_config(config, port, error), error);
    require(std::fabs(port.MouseSensitivity() - 0.3F) < 1e-6F, "port sensitivity");
    require(!port.MouseInverted(), "port inversion");

    // The shipped file moves with the arrow keys, jumps with J and crouches with the
    // right control key; the port must report those actions from the raw device codes.
    constexpr std::uint32_t vk_left = 0x25U;
    constexpr std::uint32_t vk_j = 0x4AU;
    constexpr std::uint32_t vk_rcontrol = 0xA3U;
    port.SetKeyDown(vk_left, true);
    port.SetKeyDown(vk_j, true);
    port.SetKeyDown(vk_rcontrol, true);
    port.Run();
    require(port.ActionDown(igi::retail::input::Action::move_left), "left arrow moves left");
    require(port.ActionDown(igi::retail::input::Action::jump), "J jumps");
    require(port.ActionDown(igi::retail::input::Action::crouch), "right control crouches");

    port.SetMouseButtonDown(0, true);
    port.Run();
    require(port.ActionDown(igi::retail::input::Action::fire), "left mouse button fires");
}

void test_unknown_control_is_rejected()
{
    igi::retail::config::InputConfig config;
    config.bindings.push_back(
        igi::retail::config::InputBinding{"Jump", "INPUTPORT_DEVICE_KEYBOARD", "KEY_NOPE"});
    igi::retail::input::InputPort port;
    std::string error;
    require(!igi::retail::config::apply_input_config(config, port, error),
            "an unknown key name fails instead of binding nothing");
}

void test_rejects_unknown_input()
{
    const std::vector<std::byte> not_a_program(16, std::byte{0x7FU});
    igi::retail::config::ScriptProgram program;
    std::string error;
    require(!igi::retail::config::read_script_program(not_a_program, program, error),
            "garbage bytes are rejected");
}

} // namespace

int main()
{
    if (!std::filesystem::exists(kGameRoot / "config.qvm")) {
        std::cout << "SKIP: retail game data is not installed at " << kGameRoot << '\n';
        return 0;
    }
    test_config_bindings();
    test_input_port_bindings();
    test_unknown_control_is_rejected();
    test_human_player_general();
    test_model_lod_table();
    test_rejects_unknown_input();
    std::cout << "retail config reader: ok\n";
    return 0;
}
