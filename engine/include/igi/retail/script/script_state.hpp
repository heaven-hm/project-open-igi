#pragma once

#include <cstdint>

extern "C" {

// Native: Script_PhaseIdGetE4_4F1A60 (placeholder until VERIFIED); retail VA 0x004F1A60.
// Returns byte at retail 0x0054E0E4 (writer: 0x004F1840/0x004F18C0). Image value 0xFF.
std::uint8_t Script_PhaseIdGetE4_4F1A60();

// Native: Script_PhaseIdGetE5_4F1A70 (placeholder until VERIFIED); retail VA 0x004F1A70.
// Returns byte at retail 0x0054E0E5 (writer: 0x004F1840/0x004F18C0). Image value 0xFF.
std::uint8_t Script_PhaseIdGetE5_4F1A70();

// Port globals for the two script bytes, initialised to the retail image value 0xFF.
extern std::uint8_t g_54e0e4;
extern std::uint8_t g_54e0e5;

} // extern "C"
