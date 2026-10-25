// Storage for the retail static objects that 0x004EE970 clears and fills.
//
// This file defines data only; it holds no native. Extents currently declared cover exactly what
// section 1 of 0x004EE970 (0x004EE970-0x004EEA2D) touches; docs/fidelity/evidence/0x4EE970-data-extents.md
// derives the full extents for the remaining sections and this file must be extended before those land.
#include <cstdint>

extern "C" {

std::uint32_t g_a71890[0x1000] = {}; // 0x00A71890: cleared by 0x004EE982 with ecx = 0x1000 (rep stosd), extent proven by that immediate
std::uint16_t g_54dd0c[5] = {};      // 0x0054DD0C..0x0054DD14: five consecutive word class ids stored by section 1
std::uint8_t g_54dda4 = 0xFFU;           // 0x0054DDA4
std::uint8_t g_54dda5 = 0xFFU;           // 0x0054DDA5
std::uint32_t g_54dda8 = 0xFFU;          // 0x0054DDA8

} // extern "C"
