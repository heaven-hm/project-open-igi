// Retail QHash slot-table globals 0x00567C38..0x00567C4C.
//
// The table is walked by 0x00401B20 (release) and maintained by the insert counterpart that ends at
// 0x00401B1C; the retail image holds zeros for all five words (pxw 24 @ 0x00567C38 -> all zero), so
// they are zero-initialised storage here. Declarations only: the definitions live in
// engine/src/retail/core/qhash_slot_table_globals.cpp.
#ifndef IGI_RETAIL_CORE_QHASH_SLOT_TABLE_HPP
#define IGI_RETAIL_CORE_QHASH_SLOT_TABLE_HPP

#include <cstdint>

namespace igi::retail::core {

extern std::uint32_t g_567c38; // 0x00567C38: element base used by the offset computation
extern std::uint32_t g_567c3c; // 0x00567C3C: per-slot index table
extern std::uint32_t g_567c40; // 0x00567C40: live slot count (incremented on insert, decremented on release)
extern std::uint32_t g_567c48; // 0x00567C48: element stride
extern std::uint32_t g_567c4c; // 0x00567C4C: slot link table

// 0x00567C74: static empty value object returned by 0x004F1180 when the hash lookup misses.
// The retail image leaves it zero and no evidence establishes its full extent yet; 0x20 bytes is the
// zeroed span observed around it and must be revisited when a caller of 0x004F1180 is transcribed.
extern std::uint32_t g_qhash_empty_567c74[8];

} // namespace igi::retail::core

#endif
