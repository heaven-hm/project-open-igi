// Storage for the retail QHash slot-table globals 0x00567C38..0x00567C5C.
//
// Owner judgement for 0x00567C5C: it is a gap in the contiguous 0x00567Cxx run this file already
// owns (0x00567C38/3C/40/48/4C and 0x00567C74), so it is defined here rather than in whichever
// native happened to need it first. 0x00401F60 ORs into it and 0x00401F80 AND-NOTs out of it.
//
// This file defines data only; it holds no native. The retail image bytes for all five words are
// zero (verified with pxw 24 @ 0x00567C38), so they start zeroed here. Pointer-valued words are stored
// as 32-bit values because the retail code addresses them as dwords; the candidate is x86-32 as well.
#include "igi/retail/core/qhash_slot_table.hpp"

namespace igi::retail::core {

std::uint32_t g_567c38 = 0; // 0x00567C38
std::uint32_t g_567c3c = 0; // 0x00567C3C
std::uint32_t g_567c40 = 0; // 0x00567C40
std::uint32_t g_567c48 = 0; // 0x00567C48
std::uint32_t g_567c4c = 0; // 0x00567C4C
std::uint32_t g_567c5c = 0; // 0x00567C5C: global flag word, ORed by 0x00401F60, AND-NOTed by 0x00401F80

std::uint32_t g_qhash_empty_567c74[8] = {}; // 0x00567C74, zeroed in the retail image

} // namespace igi::retail::core
