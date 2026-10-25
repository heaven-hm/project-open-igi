// Native: QHashSlotTable_Insert_401AE0; retail VA 0x00401AE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00401AE0-0x00401B1C; radare2 6.2.0 pd/afij (61 bytes, 1 block, 15 instructions); the five slot-table globals 0x00567C38..0x00567C4C are the ones the release half 0x00401B20 uses; called from 0x004F10C3 and 0x004F90FC.
// Behavior/CFG: one block, no branch. slot = index_table[live_count] * stride + element_base; link_table[slot] = live_count; ++live_count; slot[0] = object; ++object->+0x18; returns the slot address. This is the insert side whose release 0x00401B20 is transcribed in native_401b20.cpp.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 15-instruction listing; runtime UNOBSERVED. The link table is indexed by the slot
//                address with scale 1 (`mov [0x567C4C]+eax, ecx`), exactly as the release half's note requires, so the two halves agree without
//                normalising to an index. The globals are zero-initialised storage (qhash_slot_table_globals.cpp) until the retail table is built.
#include "igi/retail/core/qhash_slot_table.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

using igi::retail::core::g_567c38;
using igi::retail::core::g_567c3c;
using igi::retail::core::g_567c40;
using igi::retail::core::g_567c48;
using igi::retail::core::g_567c4c;

extern "C" {

void* QHashSlotTable_Insert_401AE0(void* object)
{
    std::uint32_t const count = g_567c40;                                            // 0x00401AE0: mov ecx,[0x567c40]
    std::uint32_t* const index_table =
        reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(g_567c3c));     // 0x00401AE6: mov eax,[0x567c3c]
    std::uint32_t const element_base = g_567c38;                                     // 0x00401AEB: mov edx,[0x567c38]
    std::uint32_t slot = index_table[count];                                         // 0x00401AF1: mov (%eax,%ecx,4),%eax
    slot *= g_567c48;                                                                // 0x00401AF4: imul [0x567c48],%eax
    slot += element_base;                                                            // 0x00401AFB: add %edx,%eax
    std::uint8_t* const links =
        reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(g_567c4c));      // 0x00401AFD: mov edx,[0x567c4c]
    *reinterpret_cast<std::uint32_t*>(links + slot) = count;                          // 0x00401B03: mov %ecx,(%edx,%eax,1)
    g_567c40 = count + 1U;                                                           // 0x00401B06/0x00401B0D: inc %ecx; mov [0x567c40],%ecx
    *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(slot)) =
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(object));        // 0x00401B13/0x00401B17: mov 0x4(%esp),%ecx; mov %ecx,(%eax)
    ++*reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(object) + 0x18U);  // 0x00401B19: inc dword (%ecx+0x18)
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(slot));               // 0x00401B1C: ret, EAX = the slot address
}
} // extern "C"

#undef NATIVE_DECL
