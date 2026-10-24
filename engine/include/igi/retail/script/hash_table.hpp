// Retail QHash bucket table 0x00A70CA8 (read by 0x004F11A0).
#ifndef IGI_RETAIL_SCRIPT_HASH_TABLE_HPP
#define IGI_RETAIL_SCRIPT_HASH_TABLE_HPP

#include <cstdint>

namespace igi::retail::script {

// 0x00A70CA8: 0xFB buckets of three dwords, indexed by (value % 0xFB) * 3.
// Extent proof: the retail initializer at 0x004EF7C5 starts at 0x00A70CA8, writes [eax], [eax+4], [eax+8]
// in 0x0C strides and stops when eax reaches 0x00A7186C, so the table is 0x00A7186C - 0x00A70CA8 = 0xBC4
// bytes = 0x2F1 dwords = 251 buckets x 3. The last bucket starts at 0x00A71860 and its third word ends
// exactly at 0x00A7186C; a 0x2EF declaration would be two dwords short.
extern std::uint32_t g_hash_buckets_a70ca8[0x2F1];

} // namespace igi::retail::script

#endif
