// Native data: the retail DirectInput runtime block. Every address is .data
// zero-fill (no file bytes, loader init 0) and runtime-written by the DirectInput
// layer natives; the port defines them so those natives can be transcribed.
#include "igi/retail/input/dinput_globals.hpp"

extern "C" {
void* g_a7b4e0[16] = {};          // 0x00A7B4E0
void* g_a7b2e0[16] = {};          // 0x00A7B2E0
void* g_a7b4f0 = nullptr;         // 0x00A7B4F0
std::int32_t g_a7b638 = 0;        // 0x00A7B638
std::uint8_t g_bcaa00[0x28 * 16] = {}; // 0x00BCAA00
std::uint8_t g_a7b440[0x28 * 16] = {}; // 0x00A7B440
void* g_5c8e1c = nullptr;      // 0x005C8E1C
std::uint8_t g_c28f80 = 0U;    // 0x00C28F80
std::uint8_t g_c28f81 = 0U;    // 0x00C28F81
} // extern "C"
