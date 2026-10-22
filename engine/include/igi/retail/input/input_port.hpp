#pragma once

// Retail input port for igi.exe.
//
// The retail game remaps the action set to the devices through config.qvm, and
// the IGIPatch project identifies the natives that implement it:
//
//   GO_InRemap          0x004037C0  adds one binding to the current profile
//   GO_InMouseInvert    0x00403870  sets the inverted mouse-look flag
//   GO_InMouseSens      0x004038F0  sets the mouse look sensitivity
//   Keyboard_Update     0x00490230  latches the keyboard state
//   Joypad_Update       0x00509CF0  latches the joystick state
//   Input_Run           0x00507EC0  latches the device state into the actions
//
// config.qvm binds them like this:
//
//   GOInRemap("MoveUp", "INPUTPORT_DEVICE_KEYBOARD", "KEY_W")
//   GOInMouSens(0.4)
//   GOInMouInv(0)
//
// The action names, the device names and the key names are the ones the retail
// script uses, so the port reads them straight out of the retail configuration
// rather than hardcoding a key map.

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace igi::retail::input {

// The in-game actions config.qvm remaps. The order is the retail script order.
enum class Action : std::uint32_t {
    move_left = 0,
    move_right,
    move_up,
    move_down,
    jump,
    crouch,
    toggle_walk_run,
    fire,
    reload,
    activate,
    next_weapon,
    previous_weapon,
    alternate_fire,
    peek,
    binoculars,
    map_computer,
    zoom_in,
    zoom_out,
    count
};

enum class Device : std::uint32_t {
    keyboard = 0,
    mouse = 1,
};

// One binding of an action to a device code.
struct Binding {
    Device device{Device::keyboard};
    std::uint32_t code{0};
};

// The per-frame action state the port latches.
struct ActionState {
    bool down{false};    // the action is held
    bool pressed{false}; // the action went down this frame
    bool released{false};
};

// The retail input port: the remap table plus the latched action state.
class InputPort {
  public:
    // GO_InRemap (0x004037C0): binds an action to a device code.
    void Remap(Action action, Device device, std::uint32_t code);

    // GO_InMouseInvert (0x00403870).
    void SetMouseInverted(bool inverted) noexcept { mouse_inverted_ = inverted; }

    // GO_InMouseSens (0x004038F0).
    void SetMouseSensitivity(float sensitivity) noexcept { mouse_sensitivity_ = sensitivity; }

    [[nodiscard]] bool MouseInverted() const noexcept { return mouse_inverted_; }
    [[nodiscard]] float MouseSensitivity() const noexcept { return mouse_sensitivity_; }

    // HOST HELPER: the platform adapter reports the raw device state.
    void SetKeyDown(std::uint32_t code, bool down);

    // HOST HELPER: the raw key state, for a key the configuration does not
    // bind to an action. The retail keeps the whole keyboard state, so a key
    // the configuration leaves out is still held.
    [[nodiscard]] bool KeyDown(std::uint32_t code) const noexcept
    {
        return raw_key_down_[code & 0x3FU];
    }
    void SetMouseButtonDown(int button, bool down);
    void AddMouseDelta(int delta_x, int delta_y);

    // Input_Run (0x00507EC0): latches the raw device state into the actions
    // and clears the per-frame change flags. The retail native keeps a 32-entry
    // state array per port and a 32-bit changed mask; the port keeps one state
    // per action and the same mask for the mouse buttons.
    void Run();

    [[nodiscard]] bool ActionDown(Action action) const noexcept;
    [[nodiscard]] bool ActionPressed(Action action) const noexcept;

    // The look delta since the last Run, already scaled by the retail mouse
    // sensitivity and the invert flag.
    [[nodiscard]] float MouseDeltaX() const noexcept { return mouse_delta_x_; }
    [[nodiscard]] float MouseDeltaY() const noexcept { return mouse_delta_y_; }

    // HOST HELPER: returns the look delta and clears it, so the consumer reads
    // the counts the device collected since the previous frame.
    std::array<float, 2> TakeMouseDelta()
    {
        const std::array<float, 2> delta{mouse_delta_x_, mouse_delta_y_};
        mouse_delta_x_ = 0.0F;
        mouse_delta_y_ = 0.0F;
        return delta;
    }

    // The bindings read out of the retail configuration, for diagnostics.
    [[nodiscard]] const std::vector<Binding>& Bindings(Action action) const noexcept {
        return bindings_[static_cast<std::size_t>(action)];
    }

  private:
    std::array<std::vector<Binding>, static_cast<std::size_t>(Action::count)> bindings_{};
    std::array<std::array<bool, 64>, static_cast<std::size_t>(Action::count)> key_down_{};
    // The raw keyboard state, for the keys the configuration does not bind.
    std::array<bool, 64> raw_key_down_{};
    std::array<std::array<bool, 64>, static_cast<std::size_t>(Action::count)> key_pressed_{};
    std::array<std::array<bool, 64>, static_cast<std::size_t>(Action::count)> key_pending_{};
    std::array<bool, 64> mouse_buttons_{};
    std::array<bool, 64> mouse_pressed_{};
    std::array<bool, 64> mouse_pending_{};
    float mouse_delta_x_{0.0F};
    float mouse_delta_y_{0.0F};
    float mouse_sensitivity_{1.0F};
    bool mouse_inverted_{false};
};

} // namespace igi::retail::input
