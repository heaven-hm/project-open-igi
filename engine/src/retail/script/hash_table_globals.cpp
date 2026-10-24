// Storage for the retail QHash bucket table at 0x00A70CA8.
//
// This file defines data only; it holds no native. The table is read by 0x004F11A0 as
// [0x00A70CA8 + (value % 0xFB) * 3 * 4], so it holds 0xFB buckets of three dwords (0x2EF dwords).
// Every word is zero in the retail image (verified with pxw 32 @ 0x00A70CA8), and the insert
// counterpart that fills it is not transcribed yet, so it starts zeroed here.
#include "igi/retail/script/hash_table.hpp"

namespace igi::retail::script {

std::uint32_t g_hash_buckets_a70ca8[0x2EF] = {}; // 0x00A70CA8, 0xFB buckets of 3 dwords

} // namespace igi::retail::script
