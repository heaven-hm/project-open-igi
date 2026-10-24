// Retail QHash bucket table 0x00A70CA8 (read by 0x004F11A0).
#ifndef IGI_RETAIL_SCRIPT_HASH_TABLE_HPP
#define IGI_RETAIL_SCRIPT_HASH_TABLE_HPP

#include <cstdint>

namespace igi::retail::script {

// 0x00A70CA8: 0xFB buckets of three dwords, indexed by (value % 0xFB) * 3.
extern std::uint32_t g_hash_buckets_a70ca8[0x2EF];

} // namespace igi::retail::script

#endif
