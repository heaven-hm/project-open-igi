#pragma once

#include <cstdint>

extern "C" {

// Retail image constants for the DirectInput layer, transcribed byte-for-byte
// from the retail image with their VAs (.rdata except the writable axis table).
extern std::uint8_t const g_guid_sys_keyboard[16];        // 0x00534900
extern std::uint8_t const g_guid_sys_mouse[16];           // 0x00534910
extern std::uint8_t const g_iid_direct_input_device2a[16]; // 0x005349B0
extern std::uint8_t const g_iid_direct_input_device_a[16]; // 0x005349C0
extern std::uint8_t const g_iid_direct_input7a[16];       // 0x005349D0
extern std::uint8_t const g_dataformat_mouse[0x18];       // 0x005348B0
extern std::uint8_t const g_dataformat_keyboard[0x18];    // 0x005348C8
extern std::uint8_t const g_dataformat_joypad[0x18];      // 0x00534C38
extern double const g_double_one_over_255;                // 0x00534258
extern double const g_double_one_over_128;                // 0x00534BB0
extern std::uint8_t g_axis_pairs[64];                     // 0x0054E8A8, writable

} // extern "C"
