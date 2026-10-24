#pragma once

// 0x009436B0: the qfile device-driver table. Five records of 0x8C bytes, established by 0x004B1180 (index allocator),
// the twelve 0x004B1BE0-family field setters, 0x004B11A0 (name scan), 0x004B1DE0 (name getter), 0x004B1B90 (present
// writer) and 0x004B1420 (device-path dispatcher). The device sub-record the dispatcher uses starts at record+0x3C, which
// is why those natives work from 0x009436EC: handler +0x00 there (= record +0x3C), present +0x1C (= +0x58), name +0x40
// (= +0x7C).
//
// 0x009436C = 0x009436B0 + 5 * 0x8C: the next-index counter, set to 1 by 0x004B1200.

#include <cstdint>

extern "C" {

// 0x009436B0: the five 0x8C-byte records. Defined in native_4b1180.cpp, which owns the counter that indexes them.
extern std::uint8_t g_9436b0[5 * 0x8C];

// 0x009436C: the next device index. Defined in native_4b1180.cpp.
extern std::int32_t g_94396c;

} // extern "C"
