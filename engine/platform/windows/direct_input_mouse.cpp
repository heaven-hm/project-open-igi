#include "direct_input_mouse.hpp"

#if defined(_WIN32)
// The retail reads the mouse through the legacy DirectInput interface: igi.exe
// imports DINPUT.dll and calls DirectInputCreateEx, and the bottle's 32-bit
// dinput.dll is the one that carries the device interface. The port takes the
// same path so the mouse behaves the way the retail's does.
// The retail asks for DirectInput version 0x800 with the 7-interface IID
// (0x9A4CB684-236D-11D3-8E9D-00C04F6844AE), which is what the bottle's
// dinput.dll provides.
#undef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>
#include <initguid.h>
#include <devguid.h>
#endif

namespace igi::platform {

#if defined(_WIN32)

namespace {

// The device class the retail asks the mouse device by. The retail builds this
// GUID at run time, so it is not in the binary as a literal, and the port
// asks for the same device.
const GUID kRetailMouseDeviceClass = {0x600d3d7b, 0xa02c, 0x3d7b,
                                      {0xa0, 0x0d, 0x3d, 0x7b, 0x60, 0x2c, 0x3d, 0x7b}};

} // namespace

DirectInputMouse::~DirectInputMouse()
{
    if (mouse_ != nullptr)
    {
        static_cast<IDirectInputDeviceA*>(mouse_)->Release();
        mouse_ = nullptr;
    }
    if (device_ != nullptr)
    {
        static_cast<IDirectInput7A*>(device_)->Release();
        device_ = nullptr;
    }
}

bool DirectInputMouse::Open()
{
    if (device_ != nullptr)
    {
        return true;
    }
    IDirectInput7A* direct_input = nullptr;
    const HRESULT created = DirectInputCreateEx(GetModuleHandleA(nullptr), 0x800,
                                                 IID_IDirectInput7A,
                                                 reinterpret_cast<void**>(&direct_input), nullptr);
    open_result_ = static_cast<unsigned long>(created);
    if (FAILED(created) || direct_input == nullptr)
    {
        return false;
    }
    // The retail initializes the input object with the module handle before it
    // asks for the mouse device.
    const HRESULT initialize_result = direct_input->Initialize(GetModuleHandleA(nullptr), DIRECTINPUT_VERSION);
    if (FAILED(initialize_result))
    {
        open_result_ = static_cast<unsigned long>(initialize_result);
        direct_input->Release();
        return false;
    }
    // The retail asks for the mouse device by the mouse device class.
    IDirectInputDeviceA* mouse = nullptr;
    const HRESULT device_result = direct_input->CreateDevice(kRetailMouseDeviceClass, &mouse, nullptr);
    open_result_ = static_cast<unsigned long>(device_result);
    if (FAILED(device_result) || mouse == nullptr)
    {
        direct_input->Release();
        return false;
    }
    const HRESULT format_result = mouse->SetDataFormat(&c_dfDIMouse);
    open_result_ = static_cast<unsigned long>(format_result);
    if (FAILED(format_result))
    {
        mouse->Release();
        direct_input->Release();
        return false;
    }
    // The retail keeps the mouse in background mode: the game reads the mouse
    // while the window is not focused, and the game hides the cursor itself.
    const HRESULT level_result =
        mouse->SetCooperativeLevel(GetDesktopWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    open_result_ = static_cast<unsigned long>(level_result);
    if (FAILED(level_result))
    {
        mouse->Release();
        direct_input->Release();
        return false;
    }
    mouse->Acquire();
    device_ = direct_input;
    mouse_ = mouse;
    first_poll_ = true;
    return true;
}

MouseCounts DirectInputMouse::Poll()
{
    MouseCounts counts;
    if (mouse_ == nullptr)
    {
        return counts;
    }
    auto* mouse = static_cast<IDirectInputDeviceA*>(mouse_);
    DIMOUSESTATE state{};
    if (mouse->GetDeviceState(sizeof(state), &state) != DI_OK)
    {
        // The device was lost, which happens when the window loses focus; the
        // retail reacquires it and reads the state on the next frame. Keep the
        // last known button state so the loss reads as no movement rather than
        // as releases.
        mouse->Acquire();
        counts.left_down = left_down_;
        counts.right_down = right_down_;
        counts.middle_down = middle_down_;
        return counts;
    }
    const auto x = static_cast<long long>(state.lX);
    const auto y = static_cast<long long>(state.lY);
    if (first_poll_)
    {
        // The first poll has no previous counts to difference against.
        first_poll_ = false;
        previous_x_ = x;
        previous_y_ = y;
    }
    else
    {
        counts.delta_x = static_cast<int>(x - previous_x_);
        counts.delta_y = static_cast<int>(y - previous_y_);
        previous_x_ = x;
        previous_y_ = y;
    }
    counts.left_down = (state.rgbButtons[0] & 0x80) != 0;
    counts.right_down = (state.rgbButtons[1] & 0x80) != 0;
    counts.middle_down = (state.rgbButtons[2] & 0x80) != 0;
    left_down_ = counts.left_down;
    right_down_ = counts.right_down;
    middle_down_ = counts.middle_down;
    counts.wheel_up = state.lZ > 0;
    counts.wheel_down = state.lZ < 0;
    return counts;
}

#else

DirectInputMouse::~DirectInputMouse() = default;

bool DirectInputMouse::Open()
{
    return false;
}

MouseCounts DirectInputMouse::Poll()
{
    return {};
}

#endif

} // namespace igi::platform
