#pragma once

// The mouse device the retail reads.
//
// igi.exe imports DINPUT.dll and takes the mouse counts from a DirectInput
// device rather than from a Win32 mouse message, so the port reads the mouse
// the same way. The device is polled once per frame and the relative counts
// it reports are handed to the retail input port.

#include <cstdint>

namespace igi::platform {

// The relative mouse counts the device collected since the previous poll.
struct MouseCounts {
    int delta_x{0};
    int delta_y{0};
    bool left_down{false};
    bool right_down{false};
    bool middle_down{false};
    bool wheel_up{false};
    bool wheel_down{false};
};

// A DirectInput mouse device, opened in background mode so the game keeps
// running when the window loses focus.
class DirectInputMouse {
  public:
    DirectInputMouse() = default;
    ~DirectInputMouse();

    DirectInputMouse(const DirectInputMouse&) = delete;
    DirectInputMouse& operator=(const DirectInputMouse&) = delete;

    // Opens the mouse device. Returns false when the device is unavailable,
    // which leaves the caller with no mouse rather than a broken one.
    bool Open();

    // Polls the device and reports the counts collected since the last poll.
    [[nodiscard]] MouseCounts Poll();

    // True when the device was opened and the mouse is usable.
    [[nodiscard]] bool Available() const noexcept { return device_ != nullptr; }

    // The result of the last Open() attempt, so the caller can report why the
    // mouse is unavailable.
    [[nodiscard]] unsigned long OpenResult() const noexcept { return open_result_; }

  private:
    void* device_{nullptr};
    void* mouse_{nullptr};
    unsigned long open_result_{0};
    long long previous_x_{0};
    long long previous_y_{0};
    bool first_poll_{true};
    bool left_down_{false};
    bool right_down_{false};
    bool middle_down_{false};
};

} // namespace igi::platform
