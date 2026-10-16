#pragma once

// Retail control bindings recovered from `/Users/haseeb-mir/Downloads/IGI/config.qvm`
// (binary QVM dump; GOPlayer / GOInRemap section). This header is the single
// source of truth for default input mapping: gameplay code queries actions via
// the helpers below, never raw InputCode values for these actions.
//
//   MoveLeft=A  MoveRight=D  MoveUp=W  MoveDown=S
//   Fire=MOUSE_BUTTON_1  Reload=KEY_F  Activate=KEY_Q  Crouch=KEY_C
//   Jump=KEY_E  NextWeapon=MOUSE_BUTTON_2  PrevWeapon=KEY_O
//   AlternateFire=KEY_BACKSPACE  Peek=KEY_NUMPAD1  Binoculars=KEY_SPACE
//   MapComputer=KEY_M  ToggleWalkRun=KEY_LEFT_SHIFT
//   ZoomIn=KEY_PAGEUP  ZoomOut=KEY_PAGEDOWN
//   WeaponCategory1..9=KEY_1..KEY_9
//
// Documented deviations (portability/playability conveniences, not retail):
// - Reload also accepts KEY_R (common FPS habit; retail is KEY_F only).
// - Crouch also accepts either Control key; Fire also accepts LeftControl.
// - MouseRight currently drives the sniper sight zoom; retail binds
//   NextWeapon to MOUSE_BUTTON_2, so weapon cycling stays on wheel/digits
//   until a retail runtime capture disambiguates scope-vs-cycle behavior.
// - Peek (numpad), ZoomIn/ZoomOut (page keys) have no InputCode yet.

#include "igi/input_state.hpp"

#include <array>
#include <cstddef>

namespace igi::gameplay::retail {

enum class InputAction : std::uint8_t {
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Fire,
    Reload,
    Activate,
    Crouch,
    Jump,
    NextWeapon,
    PrevWeapon,
    AlternateFire,
    Binoculars,
    MapComputer,
    ToggleWalkRun,
    WeaponCategory1,
    WeaponCategory2,
    WeaponCategory3,
    WeaponCategory4,
    WeaponCategory5,
    WeaponCategory6,
    WeaponCategory7,
    WeaponCategory8,
    WeaponCategory9,
};

namespace detail {

struct Binding {
    InputAction action;
    InputCode primary;
    InputCode secondary; // InputCode::Unknown when unused
};

// constexpr table mirrors config.qvm GOInRemap entries in file order.
constexpr std::array<Binding, 24> kDefaultBindings{{
    {InputAction::MoveLeft, InputCode::KeyA, InputCode::Unknown},
    {InputAction::MoveRight, InputCode::KeyD, InputCode::Unknown},
    {InputAction::MoveUp, InputCode::KeyW, InputCode::Unknown},
    {InputAction::MoveDown, InputCode::KeyS, InputCode::Unknown},
    {InputAction::Fire, InputCode::MouseLeft, InputCode::LeftControl},
    {InputAction::Reload, InputCode::KeyF, InputCode::KeyR},
    {InputAction::Activate, InputCode::KeyQ, InputCode::Unknown},
    {InputAction::Crouch, InputCode::KeyC, InputCode::LeftControl},
    {InputAction::Jump, InputCode::KeyE, InputCode::Unknown},
    {InputAction::NextWeapon, InputCode::MouseRight, InputCode::Unknown},
    {InputAction::PrevWeapon, InputCode::KeyO, InputCode::Unknown},
    {InputAction::AlternateFire, InputCode::Backspace, InputCode::Unknown},
    {InputAction::Binoculars, InputCode::Space, InputCode::Unknown},
    {InputAction::MapComputer, InputCode::KeyM, InputCode::Unknown},
    {InputAction::ToggleWalkRun, InputCode::LeftShift, InputCode::RightShift},
    {InputAction::WeaponCategory1, InputCode::Digit1, InputCode::Unknown},
    {InputAction::WeaponCategory2, InputCode::Digit2, InputCode::Unknown},
    {InputAction::WeaponCategory3, InputCode::Digit3, InputCode::Unknown},
    {InputAction::WeaponCategory4, InputCode::Digit4, InputCode::Unknown},
    {InputAction::WeaponCategory5, InputCode::Digit5, InputCode::Unknown},
    {InputAction::WeaponCategory6, InputCode::Digit6, InputCode::Unknown},
    {InputAction::WeaponCategory7, InputCode::Digit7, InputCode::Unknown},
    {InputAction::WeaponCategory8, InputCode::Digit8, InputCode::Unknown},
    {InputAction::WeaponCategory9, InputCode::Digit9, InputCode::Unknown},
}};

} // namespace detail

[[nodiscard]] inline bool is_action_down(
    const InputSnapshot& snapshot, InputAction action) noexcept {
    for (const auto& binding : detail::kDefaultBindings) {
        if (binding.action == action) {
            if (snapshot.is_down(binding.primary)) {
                return true;
            }
            if (binding.secondary != InputCode::Unknown &&
                snapshot.is_down(binding.secondary)) {
                return true;
            }
            return false;
        }
    }
    return false;
}

[[nodiscard]] inline bool was_action_pressed(
    const InputSnapshot& snapshot, InputAction action) noexcept {
    for (const auto& binding : detail::kDefaultBindings) {
        if (binding.action == action) {
            if (snapshot.was_pressed(binding.primary)) {
                return true;
            }
            if (binding.secondary != InputCode::Unknown &&
                snapshot.was_pressed(binding.secondary)) {
                return true;
            }
            return false;
        }
    }
    return false;
}

} // namespace igi::gameplay::retail
