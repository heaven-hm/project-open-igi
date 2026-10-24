#pragma once

// 0x00539540 and 0x005690D8: engine setting bytes read one at a time by the small getters transcribed in
// engine/src/retail/config/. Neither region is written by a transcribed native yet, so the storage is defined in the
// files that read it and declared here for the rest.
//
// 0x00539540 extent: the ten consecutive byte stores at 0x0041556A-0x004155D6 write 0x00539540..0x00539549, and the
// transcribed readers go up to 0x0053954E. The array here therefore covers 0x00539540..0x0053954E (0x0F bytes), the
// smallest object that contains every reference the binary makes to this region. No instruction states a size.
// 0x005690D8 extent: dword loads at 0x00407C4A and 0x00407C8F prove at least four bytes, and the region is read/written
// as a byte at 0x004071D0 and 0x004072A0; four bytes is the extent used.

#include <cstdint>

extern "C" {

// 0x00539540..0x0053954E: defined in config/native_4167d0.cpp.
extern std::uint8_t g_539540[0x0F];
// 0x005690D8: defined in config/native_4071d0.cpp.
extern std::uint32_t g_5690d8;

} // extern "C"
