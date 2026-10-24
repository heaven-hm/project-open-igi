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

} // extern "C"
