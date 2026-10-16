#include "igi/input_state.hpp"

namespace igi::gameplay {
namespace {

[[nodiscard]] bool is_valid_input_code(InputCode input_code) noexcept {
    return input_code > InputCode::Unknown && input_code < InputCode::Count;
}

[[nodiscard]] std::size_t input_code_index(InputCode input_code) noexcept {
    return static_cast<std::size_t>(input_code);
}

} // namespace

bool InputSnapshot::is_down(InputCode input_code) const noexcept {
    return is_valid_input_code(input_code) && down_controls_.test(input_code_index(input_code));
}

bool InputSnapshot::was_pressed(InputCode input_code) const noexcept {
    return is_valid_input_code(input_code) && pressed_controls_.test(input_code_index(input_code));
}

bool InputSnapshot::was_released(InputCode input_code) const noexcept {
    return is_valid_input_code(input_code) && released_controls_.test(input_code_index(input_code));
}

std::size_t InputSnapshot::down_control_count() const noexcept {
    return down_controls_.count();
}

std::size_t InputSnapshot::pressed_control_count() const noexcept {
    return pressed_controls_.count();
}

std::size_t InputSnapshot::released_control_count() const noexcept {
    return released_controls_.count();
}

bool InputState::set_control_state(InputCode input_code, bool is_down) noexcept {
    if (!is_valid_input_code(input_code)) {
        return false;
    }

    const std::size_t control_index = input_code_index(input_code);
    const bool was_down = snapshot_.down_controls_.test(control_index);
    if (was_down == is_down) {
        return true;
    }

    snapshot_.down_controls_.set(control_index, is_down);
    if (is_down) {
        snapshot_.pressed_controls_.set(control_index);
    } else {
        snapshot_.released_controls_.set(control_index);
    }
    return true;
}

InputSnapshot InputState::consume_tick() noexcept {
    InputSnapshot consumed_snapshot = snapshot_;
    snapshot_.pressed_controls_.reset();
    snapshot_.released_controls_.reset();
    return consumed_snapshot;
}

InputSnapshot InputState::current_snapshot() const noexcept {
    return snapshot_;
}

void InputState::reset() noexcept {
    snapshot_ = {};
}

} // namespace igi::gameplay
