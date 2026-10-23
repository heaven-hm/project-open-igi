#pragma once

// The game's main method.
//
// igi.exe starts at 0x0048F530: it takes the IGIMUTEX mutex, initialises
// COM, registers the window class with the window procedure at 0x0048F970,
// reads the configuration and the script setup, registers the task classes
// and then runs the per-frame loop at 0x0048F80D.
//
// The original main also brings up systems the MVP demo does not model: the
// movie and subtitle playback, the smoke and particle emitters, the debug
// key handlers and the sound and joystick backends. Their setup calls sit
// between the script setup and the class registrations, and this
// transcription skips them, exactly as the MVP scope requires. The order of
// the calls that remain is the original's.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::core {

// The inputs the per-frame body needs: the registered instance, the window
// class, the show command and the level/display selection.
struct MainConfig
{
    void* window_class{};
    void* instance{};
    int show_command{};
    std::uint32_t level_number{1U};
    bool windowed{false};
};

} // namespace igi::retail::core

extern "C" {

// Native: Main() - VA 0x0048F530 - complete port.
// Behavior: creates the IGIMUTEX mutex with the initial owner and leaves when
//           the mutex already exists or the handle is invalid, initialises
//           COM, registers the window class with the window procedure at
//           0x0048F970, sets the startup app-context flags
//           (AppContext_SetIsActive(1) at 0x0048F615 and
//           AppContext_SetTerrainLightmapsUsed(1) at 0x0048F63D), reads the
//           configuration and the script setup,
//           registers the Window, WinInput, Germany and USA task classes
//           through the class registration helper at 0x0048F360, and runs the
//           per-frame loop: the x87 precision setup, the app context's
//           isActive flag, the frame dispatch or a 500 ms sleep, the loop
//           status byte and the message pump.
int IGI_RETAIL_CDECL Main(const igi::retail::core::MainConfig* config);

} // extern "C"

#undef IGI_RETAIL_CDECL
