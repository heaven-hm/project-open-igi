#pragma once

#include <cstdint>

extern "C" {

// Retail DirectInput data block (all .data zero-fill, runtime-written; image init 0).
extern void* g_a7b4e0[16];       // retail 0x00A7B4E0, per-slot device-present flags
extern void* g_a7b2e0[16];       // retail 0x00A7B2E0, parallel device pointers
extern void* g_a7b4f0;           // retail 0x00A7B4F0, DirectInput object (joypad path)
extern std::int32_t g_a7b638;    // retail 0x00A7B638, device count
extern std::uint8_t g_bcaa00[0x28 * 16]; // retail 0x00BCAA00, per-slot action state
extern std::uint8_t g_a7b440[0x28 * 16]; // retail 0x00A7B440, per-slot action source bytes
extern void* g_5c8e1c;          // retail 0x005C8E1C, IDirectInputDeviceA* (mouse)
extern std::uint8_t g_c28f80;   // retail 0x00C28F80, mouse-capabilities valid flag
extern std::uint8_t g_c28f81;   // retail 0x00C28F81, mouse device type byte
extern std::int32_t g_5c8e10;   // retail 0x005C8E10, left button state
extern std::int32_t g_5c8e0c;   // retail 0x005C8E0C, right button state
extern std::int32_t g_5c8e18;   // retail 0x005C8E18, middle button state
extern std::int32_t g_5c8e08;   // retail 0x005C8E08, accumulated wheel delta

} // extern "C"
