#include "test_framework.hpp"

#include "igi/input_state.hpp"
#include "igi/retail_controls.hpp"

using namespace igi::test;
using igi::gameplay::InputCode;
using igi::gameplay::InputState;
using igi::gameplay::retail::InputAction;
using igi::gameplay::retail::is_action_down;
using igi::gameplay::retail::was_action_pressed;

namespace {

InputState pressed_state(InputCode code) {
    InputState state;
    (void)state.set_control_state(code, true);
    return state;
}

} // namespace

TEST_CASE(RetailControls, JumpIsKeyEPerConfigQvm) {
    ASSERT_TRUE(was_action_pressed(pressed_state(InputCode::KeyE).consume_tick(), InputAction::Jump));
    ASSERT_FALSE(
        was_action_pressed(pressed_state(InputCode::Space).consume_tick(), InputAction::Jump));
}

TEST_CASE(RetailControls, SpaceIsBinocularsNotJump) {
    ASSERT_TRUE(was_action_pressed(
        pressed_state(InputCode::Space).consume_tick(), InputAction::Binoculars));
}

TEST_CASE(RetailControls, ReloadIsKeyFWithRConvenience) {
    ASSERT_TRUE(was_action_pressed(
        pressed_state(InputCode::KeyF).consume_tick(), InputAction::Reload));
    ASSERT_TRUE(was_action_pressed(
        pressed_state(InputCode::KeyR).consume_tick(), InputAction::Reload));
}

TEST_CASE(RetailControls, ActivateIsKeyQ) {
    ASSERT_TRUE(was_action_pressed(
        pressed_state(InputCode::KeyQ).consume_tick(), InputAction::Activate));
    ASSERT_FALSE(was_action_pressed(
        pressed_state(InputCode::KeyF).consume_tick(), InputAction::Activate));
    ASSERT_FALSE(was_action_pressed(
        pressed_state(InputCode::KeyE).consume_tick(), InputAction::Activate));
}

TEST_CASE(RetailControls, FireCrouchMoveBindings) {
    ASSERT_TRUE(is_action_down(
        pressed_state(InputCode::MouseLeft).consume_tick(), InputAction::Fire));
    ASSERT_TRUE(is_action_down(
        pressed_state(InputCode::KeyC).consume_tick(), InputAction::Crouch));
    ASSERT_TRUE(is_action_down(
        pressed_state(InputCode::KeyW).consume_tick(), InputAction::MoveUp));
    ASSERT_TRUE(is_action_down(
        pressed_state(InputCode::KeyS).consume_tick(), InputAction::MoveDown));
    ASSERT_TRUE(is_action_down(
        pressed_state(InputCode::KeyA).consume_tick(), InputAction::MoveLeft));
    ASSERT_TRUE(is_action_down(
        pressed_state(InputCode::KeyD).consume_tick(), InputAction::MoveRight));
    ASSERT_TRUE(is_action_down(
        pressed_state(InputCode::LeftShift).consume_tick(), InputAction::ToggleWalkRun));
    ASSERT_TRUE(is_action_down(
        pressed_state(InputCode::RightShift).consume_tick(), InputAction::ToggleWalkRun));
}

TEST_CASE(RetailControls, WeaponCategoryAndPrevWeaponBindings) {
    ASSERT_TRUE(was_action_pressed(
        pressed_state(InputCode::Digit1).consume_tick(), InputAction::WeaponCategory1));
    ASSERT_TRUE(was_action_pressed(
        pressed_state(InputCode::Digit5).consume_tick(), InputAction::WeaponCategory5));
    ASSERT_TRUE(was_action_pressed(
        pressed_state(InputCode::KeyO).consume_tick(), InputAction::PrevWeapon));
    ASSERT_TRUE(is_action_down(
        pressed_state(InputCode::MouseRight).consume_tick(), InputAction::NextWeapon));
}

TEST_CASE(RetailControls, BindingTableCoversAllRetailActions) {
    ASSERT_EQ(igi::gameplay::retail::detail::kDefaultBindings.size(), 24u);
}
