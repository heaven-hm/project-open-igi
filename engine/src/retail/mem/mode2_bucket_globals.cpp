// Static data for the mode-2 bucket heap: retail 0x005446B0 sentinel, 0x005466D0, 0x00936200.
//
// This file defines storage only; it holds no native. The sentinel bytes were read from the retail
// image (pxw 24 @ 0x005446B0 -> 005446B0 005446B0 005446C8 005446C8 FFFFFFFF FFFFFFFF), and every
// pointer is initialised candidate-relative so the linker emits relocations instead of baking retail
// addresses into a dynamically based candidate image.
#include "igi/retail/mem/mode2_bucket.hpp"

namespace igi::retail::mem {

Mode2BucketNode g_5446b0 = {
    &g_5446b0,                     // 0x005446B0: prev, self in the image
    &g_5446b0,                     // 0x005446B4: next, self in the image
    &g_5446b0.page_entries[0],     // 0x005446B8: 0x005446C8 = sentinel + 0x18
    static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&g_5446b0.page_entries[0])), // 0x005446BC: same address
    reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU)), // 0x005446C0: -1 marks the sentinel
    reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU)), // 0x005446C4: -1 marks the sentinel
    {},
};

Mode2BucketNode* g_5466d0 = nullptr; // 0x005466D0
std::int32_t g_936200 = 0;           // 0x00936200

} // namespace igi::retail::mem
