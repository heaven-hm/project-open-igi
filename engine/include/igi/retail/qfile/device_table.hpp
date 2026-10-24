#pragma once

// 0x009436EC: the retail device table. Five records of 0x8C bytes, established by 0x004B1420 (dispatch),
// 0x004B11A0 (scan and name compare) and 0x004B1DE0 (record +0x40 name pointer). Only three offsets are
// observed by transcribed code so far: +0x00 the dispatch handler, +0x1C the present word, +0x40 the
// NUL-terminated device name. The rest stays padding and is deliberately unnamed.
//
// The retail writer of +0x1C is 0x004B1B90, which is NOT transcribed yet, so this storage is port-owned and
// starts zeroed; until that registrar lands every dispatch takes the "Device not present" path.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

struct DeviceRecord
{
    std::int32_t (NATIVE_DECL* handler)(std::int32_t index, char* name, void* callback, std::int32_t context); // +0x00
    std::uint8_t unknown_0x04[0x18];   // +0x04
    std::int32_t present;              // +0x1C
    std::uint8_t unknown_0x20[0x20];   // +0x20
    char name[0x40];                   // +0x40
    std::uint8_t unknown_0x80[0x0C];   // +0x80
};

static_assert(sizeof(DeviceRecord) == 0x8C, "device record stride must be 0x8C");

extern "C" {

// 0x009436EC: storage defined in native_4b1de0.cpp.
extern DeviceRecord g_9436ec[5];

} // extern "C"

#undef NATIVE_DECL
