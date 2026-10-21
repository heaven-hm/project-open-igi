#include "igi/retail/input/input_port.hpp"

#include <cstddef>

namespace igi::retail::input {
void InputPort::Remap(Action action, Device device, std::uint32_t code)
{
    auto& list = bindings_[static_cast<std::size_t>(action)];
    for (Binding& binding : list) {
        if (binding.device == device && binding.code == code) {
            return;
        }
    }
    list.push_back(Binding{device, code});
}

void InputPort::SetKeyDown(std::uint32_t code, bool down)
{
    for (std::size_t index = 0; index < bindings_.size(); ++index) {
        for (const Binding& binding : bindings_[index]) {
            if (binding.device != Device::keyboard || binding.code != code) {
                continue;
            }
            const std::size_t slot = binding.code & 0x3FU;
            if (down) {
                key_pending_[index][slot] = true;
            }
            key_down_[index][slot] = down;
        }
    }
}

void InputPort::SetMouseButtonDown(int button, bool down)
{
    if (button < 0 || button >= static_cast<int>(mouse_buttons_.size())) {
        return;
    }
    const std::size_t slot = static_cast<std::size_t>(button);
    if (down) {
        mouse_pending_[slot] = true;
    }
    mouse_buttons_[slot] = down;
    for (std::size_t index = 0; index < bindings_.size(); ++index) {
        for (const Binding& binding : bindings_[index]) {
            if (binding.device != Device::mouse ||
                static_cast<int>(binding.code) != button) {
                continue;
            }
            if (down) {
                key_pending_[index][slot] = true;
            }
            key_down_[index][slot] = down;
        }
    }
}

void InputPort::AddMouseDelta(int delta_x, int delta_y)
{
    const float scale = mouse_sensitivity_;
    mouse_delta_x_ += static_cast<float>(delta_x) * scale;
    mouse_delta_y_ += static_cast<float>(delta_y) * scale * (mouse_inverted_ ? -1.0F : 1.0F);
}

void InputPort::Run()
{
    // The retail native keeps a 32-bit changed mask beside the state array and
    // clears it as it runs. The port publishes the edges collected since the
    // previous run and clears the collection, so a pressed action stays visible
    // for exactly one frame.
    for (std::size_t index = 0; index < key_pressed_.size(); ++index) {
        key_pressed_[index] = key_pending_[index];
        key_pending_[index].fill(false);
    }
    mouse_pressed_ = mouse_pending_;
    mouse_pending_.fill(false);
}

bool InputPort::ActionDown(Action action) const noexcept
{
    const std::size_t index = static_cast<std::size_t>(action);
    for (const Binding& binding : bindings_[index]) {
        const auto& held = binding.device == Device::keyboard
                               ? key_down_[index][binding.code & 0x3FU]
                               : mouse_buttons_[binding.code & 0x3FU];
        if (held) {
            return true;
        }
    }
    return false;
}

bool InputPort::ActionPressed(Action action) const noexcept
{
    const std::size_t index = static_cast<std::size_t>(action);
    for (const Binding& binding : bindings_[index]) {
        const auto& pressed = binding.device == Device::keyboard
                                 ? key_pressed_[index][binding.code & 0x3FU]
                                 : mouse_pressed_[binding.code & 0x3FU];
        if (pressed) {
            return true;
        }
    }
    return false;
}

} // namespace igi::retail::input
