#include "igi/retail/config/retail_input_bindings.hpp"

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

// HOST HELPER: config.qvm bindings for the retail input port.
// Artifact: config.qvm from the retail install.
// Tools/evidence: the decoded GOInRemap rows in docs/fidelity/evidence/agent-c-config-20261020/config.decompiled.qsc.
// Behavior/CFG: maps decoded names to input::Action, input::Device and Win32 virtual-key codes; no native is executed.
// Status/limits: covers the names the shipped file uses; an unknown action, device or control fails the read.

namespace igi::retail::config
{
namespace
{

// Win32 virtual-key codes. The numbers are fixed by the platform ABI, so the adapter
// does not need <windows.h> and keeps the module buildable on the host.
constexpr std::uint32_t vk_space = 0x20U;
constexpr std::uint32_t vk_prior = 0x21U;
constexpr std::uint32_t vk_next = 0x22U;
constexpr std::uint32_t vk_home = 0x24U;
constexpr std::uint32_t vk_end = 0x23U;
constexpr std::uint32_t vk_left = 0x25U;
constexpr std::uint32_t vk_up = 0x26U;
constexpr std::uint32_t vk_right = 0x27U;
constexpr std::uint32_t vk_down = 0x28U;
constexpr std::uint32_t vk_insert = 0x2DU;
constexpr std::uint32_t vk_delete = 0x2EU;
constexpr std::uint32_t vk_escape = 0x1BU;
constexpr std::uint32_t vk_tab = 0x09U;
constexpr std::uint32_t vk_return = 0x0DU;
constexpr std::uint32_t vk_back = 0x08U;
constexpr std::uint32_t vk_shift = 0x10U;
constexpr std::uint32_t vk_control = 0x11U;
constexpr std::uint32_t vk_menu = 0x12U;
constexpr std::uint32_t vk_lshift = 0xA0U;
constexpr std::uint32_t vk_rshift = 0xA1U;
constexpr std::uint32_t vk_lcontrol = 0xA2U;
constexpr std::uint32_t vk_rcontrol = 0xA3U;
constexpr std::uint32_t vk_lmenu = 0xA4U;
constexpr std::uint32_t vk_rmenu = 0xA5U;
constexpr std::uint32_t vk_numpad0 = 0x60U;
constexpr std::uint32_t vk_f1 = 0x70U;

// The special-key names the retail configuration uses. Single letters, digits and the
// F-keys are handled by the patterns below so a rebind to any of them still resolves.
struct NamedKey
{
    std::string_view name;
    std::uint32_t code;
};

constexpr std::array<NamedKey, 22> named_keys{{
    {"KEY_LEFT", vk_left},
    {"KEY_RIGHT", vk_right},
    {"KEY_UP", vk_up},
    {"KEY_DOWN", vk_down},
    {"KEY_SPACE", vk_space},
    {"KEY_PAGEUP", vk_prior},
    {"KEY_PAGEDOWN", vk_next},
    {"KEY_HOME", vk_home},
    {"KEY_END", vk_end},
    {"KEY_INSERT", vk_insert},
    {"KEY_DELETE", vk_delete},
    {"KEY_ESCAPE", vk_escape},
    {"KEY_TAB", vk_tab},
    {"KEY_ENTER", vk_return},
    {"KEY_BACKSPACE", vk_back},
    {"KEY_SHIFT", vk_shift},
    {"KEY_CTRL", vk_control},
    {"KEY_ALT", vk_menu},
    {"KEY_LEFT_SHIFT", vk_lshift},
    {"KEY_RIGHT_SHIFT", vk_rshift},
    {"KEY_LEFT_CTRL", vk_lcontrol},
    {"KEY_RIGHT_CTRL", vk_rcontrol},
}};

std::optional<std::uint32_t> key_code(std::string_view name)
{
    for (const NamedKey& key : named_keys) {
        if (name == key.name) {
            return key.code;
        }
    }
    if (name == "KEY_LEFT_ALT") {
        return vk_lmenu;
    }
    if (name == "KEY_RIGHT_ALT") {
        return vk_rmenu;
    }
    // KEY_A .. KEY_Z and KEY_0 .. KEY_9 use the ASCII code as the virtual key.
    if (name.size() == 5U && name.substr(0U, 4U) == "KEY_") {
        const char letter = name[4U];
        if ((letter >= 'A' && letter <= 'Z') || (letter >= '0' && letter <= '9')) {
            return static_cast<std::uint32_t>(letter);
        }
    }
    // KEY_F1 .. KEY_F12.
    if (name.size() >= 6U && name.substr(0U, 5U) == "KEY_F") {
        const std::string_view number = name.substr(5U);
        if (number.size() <= 2U) {
            unsigned int value = 0U;
            for (const char digit : number) {
                if (digit < '0' || digit > '9') {
                    return std::nullopt;
                }
                value = (value * 10U) + static_cast<unsigned int>(digit - '0');
            }
            if (value >= 1U && value <= 12U) {
                return vk_f1 + (value - 1U);
            }
        }
    }
    // KEY_NUMPAD0 .. KEY_NUMPAD9.
    if (name.size() == 11U && name.substr(0U, 10U) == "KEY_NUMPAD") {
        const char digit = name[10U];
        if (digit >= '0' && digit <= '9') {
            return vk_numpad0 + static_cast<std::uint32_t>(digit - '0');
        }
    }
    return std::nullopt;
}

std::optional<input::Action> action_of(std::string_view name)
{
    if (name == "MoveLeft") {
        return input::Action::move_left;
    }
    if (name == "MoveRight") {
        return input::Action::move_right;
    }
    if (name == "MoveUp") {
        return input::Action::move_up;
    }
    if (name == "MoveDown") {
        return input::Action::move_down;
    }
    if (name == "Jump") {
        return input::Action::jump;
    }
    if (name == "Crouch") {
        return input::Action::crouch;
    }
    if (name == "ToggleWalkRun") {
        return input::Action::toggle_walk_run;
    }
    if (name == "Fire") {
        return input::Action::fire;
    }
    if (name == "Reload") {
        return input::Action::reload;
    }
    if (name == "Activate") {
        return input::Action::activate;
    }
    if (name == "NextWeapon") {
        return input::Action::next_weapon;
    }
    if (name == "PrevWeapon") {
        return input::Action::previous_weapon;
    }
    if (name == "AlternateFire") {
        return input::Action::alternate_fire;
    }
    if (name == "Peek") {
        return input::Action::peek;
    }
    if (name == "Binoculars") {
        return input::Action::binoculars;
    }
    if (name == "MapComputer") {
        return input::Action::map_computer;
    }
    if (name == "ZoomIn") {
        return input::Action::zoom_in;
    }
    if (name == "ZoomOut") {
        return input::Action::zoom_out;
    }
    // The weapon-category bindings and the remaining configuration actions are outside
    // the movement scope; they are decoded but not bound here.
    return std::nullopt;
}

} // namespace

bool apply_input_config(const InputConfig& config, input::InputPort& port, std::string& error)
{
    struct Resolved
    {
        input::Action action;
        input::Device device;
        std::uint32_t code;
    };
    std::vector<Resolved> resolved;
    resolved.reserve(config.bindings.size());
    for (const InputBinding& binding : config.bindings) {
        const std::optional<input::Action> action = action_of(binding.action);
        if (!action.has_value()) {
            if (binding.action.rfind("WeaponCategory", 0U) == 0U) {
                continue;
            }
            error = "config.qvm binds an action the port does not define: " + binding.action;
            return false;
        }

        input::Device device = input::Device::keyboard;
        std::uint32_t code = 0U;
        if (binding.device == "INPUTPORT_DEVICE_KEYBOARD") {
            const std::optional<std::uint32_t> resolved_code = key_code(binding.control);
            if (!resolved_code.has_value()) {
                error = "config.qvm binds a key the adapter does not know: " + binding.control;
                return false;
            }
            device = input::Device::keyboard;
            code = *resolved_code;
        } else if (binding.device == "INPUTPORT_DEVICE_MOUSE") {
            if (binding.control == "MOUSE_BUTTON_1") {
                code = 0U;
            } else if (binding.control == "MOUSE_BUTTON_2") {
                code = 1U;
            } else if (binding.control == "MOUSE_BUTTON_3") {
                code = 2U;
            } else {
                error = "config.qvm binds a mouse button the adapter does not know: "
                    + binding.control;
                return false;
            }
            device = input::Device::mouse;
        } else {
            error = "config.qvm binds a device the adapter does not know: " + binding.device;
            return false;
        }
        resolved.push_back(Resolved{*action, device, code});
    }

    for (const Resolved& binding : resolved) {
        port.Remap(binding.action, binding.device, binding.code);
    }
    if (config.mouse_sensitivity.has_value()) {
        port.SetMouseSensitivity(*config.mouse_sensitivity);
    }
    if (config.mouse_inverted.has_value()) {
        port.SetMouseInverted(*config.mouse_inverted != 0);
    }
    return true;
}

bool apply_input_config_file(const std::filesystem::path& path, input::InputPort& port,
                             std::string& error)
{
    ScriptProgram program;
    if (!read_script_file(path, program, error)) {
        return false;
    }
    InputConfig config;
    if (!decode_input_config(program, config, error)) {
        return false;
    }
    return apply_input_config(config, port, error);
}

} // namespace igi::retail::config
