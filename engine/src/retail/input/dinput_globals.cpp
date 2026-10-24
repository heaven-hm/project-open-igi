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
std::int32_t g_5c8e10 = 0;     // 0x005C8E10
std::int32_t g_5c8e0c = 0;     // 0x005C8E0C
std::int32_t g_5c8e18 = 0;     // 0x005C8E18
std::int32_t g_5c8e08 = 0;     // 0x005C8E08
std::uint8_t g_c28e80[256] = {}; // 0x00C28E80
void* g_5c8e14 = nullptr;      // 0x005C8E14
void* g_5c8e24 = nullptr;      // 0x005C8E24
void* g_5c8e28 = nullptr;      // 0x005C8E28
std::int32_t g_5c8e30 = 0;     // 0x005C8E30
std::uint8_t g_c28c62[256] = {}; // 0x00C28C62
std::uint8_t g_c28d62[256] = {}; // 0x00C28D62
float g_c28f84 = 0.0F;         // 0x00C28F84
float g_c28f88 = 0.0F;         // 0x00C28F88
std::int32_t g_c28f8c = 0;     // 0x00C28F8C
std::uint8_t g_a7b2f0[16 * 0x50] = {}; // 0x00A7B2F0
std::uint8_t g_a7b4f8[16 * 0x50] = {}; // 0x00A7B4F8
std::int32_t g_a7b63c = 0;     // 0x00A7B63C
std::uint8_t g_a7b430[16 * 0x2C] = {}; // 0x00A7B430
} // extern "C"
