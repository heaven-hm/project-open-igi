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
extern std::uint8_t g_c28e80[256]; // retail 0x00C28E80, keyboard scan-code state
extern void* g_5c8e14;          // retail 0x005C8E14, IDirectInput7A* (mouse path)
extern void* g_5c8e24;          // retail 0x005C8E24, keyboard IDirectInputDeviceA*
extern void* g_5c8e28;          // retail 0x005C8E28, keyboard IDirectInputA*
extern std::int32_t g_5c8e30;   // retail 0x005C8E30, keyboard acquire latch
extern std::uint8_t g_c28c62[256]; // retail 0x00C28C62, previous keyboard state copy
extern std::uint8_t g_c28d62[256]; // retail 0x00C28D62, current keyboard state copy
extern float g_c28f84;          // retail 0x00C28F84, accumulated mouse x
extern float g_c28f88;          // retail 0x00C28F88, accumulated mouse y
extern std::int32_t g_c28f8c;   // retail 0x00C28F8C, built mouse button mask
extern std::uint8_t g_a7b2f0[16 * 0x50]; // retail 0x00A7B2F0, joypad state snapshot source
extern std::uint8_t g_a7b4f8[16 * 0x50]; // retail 0x00A7B4F8, joypad state snapshot
extern std::int32_t g_a7b63c;   // retail 0x00A7B63C, joypad deadzone value
extern std::uint8_t g_a7b430[16 * 0x2C]; // retail 0x00A7B430, per-device caps blocks

} // extern "C"
