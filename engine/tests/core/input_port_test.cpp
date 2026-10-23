#include "igi/retail/input/input_port.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// The retail config.qvm binds the actions to these keys.
constexpr std::uint32_t key_a = 0x41;
constexpr std::uint32_t key_c = 0x43;
constexpr std::uint32_t key_e = 0x45;
constexpr std::uint32_t key_w = 0x57;

void test_remap_binds_actions()
{
    igi::retail::input::InputPort port;
    port.Remap(igi::retail::input::Action::move_up, igi::retail::input::Device::keyboard, key_w);
    port.Remap(igi::retail::input::Action::jump, igi::retail::input::Device::keyboard, key_e);
    port.Remap(igi::retail::input::Action::crouch, igi::retail::input::Device::keyboard, key_c);

    require(port.Bindings(igi::retail::input::Action::move_up).size() == 1U,
            "the walk action keeps one binding");
    require(port.Bindings(igi::retail::input::Action::fire).empty(),
            "an unbound action has no bindings");
}

void test_a_key_only_presses_its_own_action()
{
    igi::retail::input::InputPort port;
    port.Remap(igi::retail::input::Action::move_up, igi::retail::input::Device::keyboard, key_w);
    port.Remap(igi::retail::input::Action::jump, igi::retail::input::Device::keyboard, key_e);
    port.Remap(igi::retail::input::Action::crouch, igi::retail::input::Device::keyboard, key_c);

    // A key that is bound to one action must not press the others, even when
    // the actions share the same slot index.
    port.SetKeyDown(key_w, true);
    port.Run();
    require(port.ActionDown(igi::retail::input::Action::move_up),
            "the walk action is held while W is down");
    require(!port.ActionDown(igi::retail::input::Action::jump),
            "the jump action is not held while W is down");
    require(!port.ActionDown(igi::retail::input::Action::crouch),
            "the crouch action is not held while W is down");

    port.SetKeyDown(key_w, false);
    port.Run();
    require(!port.ActionDown(igi::retail::input::Action::move_up),
            "the walk action is released when W comes up");
}

void test_pressed_edge_lasts_one_run()
{
    igi::retail::input::InputPort port;
    port.Remap(igi::retail::input::Action::jump, igi::retail::input::Device::keyboard, key_e);

    port.SetKeyDown(key_e, true);
    port.Run();
    require(port.ActionPressed(igi::retail::input::Action::jump),
            "the press edge is visible after the run");

    port.Run();
    require(!port.ActionPressed(igi::retail::input::Action::jump),
            "the press edge is cleared by the next run");
}

void test_mouse_delta_uses_the_retail_sensitivity()
{
    igi::retail::input::InputPort port;
    port.SetMouseSensitivity(0.4F);
    port.AddMouseDelta(10, 5);
    const auto delta = port.TakeMouseDelta();
    require(delta[0] > 3.9F && delta[0] < 4.1F, "the look delta carries the retail sensitivity");
    require(delta[1] > 1.9F && delta[1] < 2.1F, "the vertical look delta carries the sensitivity");

    const auto second = port.TakeMouseDelta();
    require(second[0] == 0.0F && second[1] == 0.0F, "taking the delta clears it");
}

void test_mouse_invert_flips_the_vertical_delta()
{
    igi::retail::input::InputPort port;
    port.SetMouseSensitivity(1.0F);
    port.SetMouseInverted(true);
    port.AddMouseDelta(0, 5);
    const auto delta = port.TakeMouseDelta();
    require(delta[1] < 0.0F, "the inverted flag flips the vertical look delta");
}

} // namespace

int main()
{
    test_remap_binds_actions();
    test_a_key_only_presses_its_own_action();
    test_pressed_edge_lasts_one_run();
    test_mouse_delta_uses_the_retail_sensitivity();
    test_mouse_invert_flips_the_vertical_delta();
    std::cout << "igi_retail_input_port_tests passed\n";
    return 0;
}
