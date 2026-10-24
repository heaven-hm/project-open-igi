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
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// Retail entry point 0x0048F530. The retail function ends with
// `add esp,0x464; retn 0x10`, i.e. a __stdcall four-argument entry called by the
// CRT start-up: (hInstance, hPrevInstance, lpCmdLine, nShowCmd). No wrapper type
// is introduced here; the arguments are passed exactly as the retail code reads
// them at [esp+0x478], [esp+0x47C], [esp+0x480] and [esp+0x484].
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
int __stdcall Main(void* h_instance, void* h_previous_instance, char* command_line, int show_command);

} // extern "C"

#undef NATIVE_DECL
