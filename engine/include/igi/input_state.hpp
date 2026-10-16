#pragma once

#include <bitset>
#include <cstddef>
#include <cstdint>

namespace igi::gameplay {

enum class InputCode : std::uint8_t {
    Unknown,
    KeyA, KeyB, KeyC, KeyD, KeyE, KeyF, KeyG, KeyH, KeyI, KeyJ, KeyK, KeyL,
    KeyM, KeyN, KeyO, KeyP, KeyQ, KeyR, KeyS, KeyT, KeyU, KeyV, KeyW, KeyX,
    KeyY, KeyZ,
    Digit0, Digit1, Digit2, Digit3, Digit4, Digit5, Digit6, Digit7, Digit8,
    Digit9,
    Return, Escape, Space, Tab, Backspace,
    LeftShift, RightShift, LeftControl, RightControl, LeftAlt, RightAlt,
    ArrowLeft, ArrowRight, ArrowDown, ArrowUp,
    MouseLeft, MouseRight, MouseMiddle,
    MouseWheelUp, MouseWheelDown,
    Count,
};

class InputState;

class InputSnapshot final {
public:
    [[nodiscard]] bool is_down(InputCode input_code) const noexcept;
    [[nodiscard]] bool was_pressed(InputCode input_code) const noexcept;
    [[nodiscard]] bool was_released(InputCode input_code) const noexcept;
    [[nodiscard]] std::size_t down_control_count() const noexcept;
    [[nodiscard]] std::size_t pressed_control_count() const noexcept;
    [[nodiscard]] std::size_t released_control_count() const noexcept;

private:
    friend class InputState;

    std::bitset<static_cast<std::size_t>(InputCode::Count)> down_controls_{};
    std::bitset<static_cast<std::size_t>(InputCode::Count)> pressed_controls_{};
    std::bitset<static_cast<std::size_t>(InputCode::Count)> released_controls_{};
};

class InputState final {
public:
    [[nodiscard]] bool set_control_state(InputCode input_code, bool is_down) noexcept;
    [[nodiscard]] InputSnapshot consume_tick() noexcept;
    [[nodiscard]] InputSnapshot current_snapshot() const noexcept;
    void reset() noexcept;

private:
    InputSnapshot snapshot_{};
};

} // namespace igi::gameplay
