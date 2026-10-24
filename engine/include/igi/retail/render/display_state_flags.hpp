#pragma once

#include <cstdint>

extern "C" {

// Native: Display_SetVSyncFlag_490340; retail VA 0x00490340. byte[0x00C28C61] = 1.
void Display_SetVSyncFlag_490340();

// Native: Display_SetSwapFlag_490350; retail VA 0x00490350. byte[0x00C28C60] = (uint8)arg1.
void Display_SetSwapFlag_490350(char flag);

// Retail .data zero-fill bytes (image init 0), runtime-written.
extern std::uint8_t g_c28c61; // 0x00C28C61
extern std::uint8_t g_c28c60; // 0x00C28C60

} // extern "C"
