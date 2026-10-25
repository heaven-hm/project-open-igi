// Native: TaskType_Inherits_401CF0; retail VA 0x00401CF0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00401CF0-0x00401D20; radare2 6.2.0 pd/afij (49 bytes, 5 blocks, 16 instructions); the 0x30-byte type slot stride comes from `and $0xffff,%eax; lea (%eax,%eax,2),%eax; shl $0x4,%eax` and the parent word sits at slot+0x08 (TaskTypeSlot::parent_type); called from 0x004F9137 and 0x004F9180.
// Behavior/CFG: five blocks, one loop over the parent chain. Compares the low word of the first argument with the second argument; on a match returns 1. Otherwise it walks to the parent word at type_table + (type*0x30) + 0x08, returns 0 when that parent is the 0x0181 root, and loops while it is neither the root nor the searched ancestor.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 16-instruction listing; runtime UNOBSERVED. Only AX of the first argument and CX
//                of the second are read (0x00401CF9/0x00401D10/0x00401D16), so the caller's dirty high halves are unobservable; the address is
//                expressed through QTask_TypeTable (task_types.hpp) rather than the 0x00AF5EE8 immediate so the port keeps working off 32-bit.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::uint8_t NATIVE_DECL TaskType_Inherits_401CF0(std::uint32_t type, std::uint16_t ancestor)
{
    std::uint8_t const* const type_table =
        reinterpret_cast<std::uint8_t const*>(&::QTask_TypeTable[0]);                // 0x00401D09: mov 0xaf5ee8(%eax),%ax (0x00AF5EE0 + 0x08)
    std::uint32_t current = type;                                                    // 0x00401CF0: mov 0x4(%esp),%eax
    for (;;)
    {
        if (static_cast<std::uint16_t>(current) == ancestor)                         // 0x00401CF9: cmp %cx,%ax; 0x00401D16: cmp %cx,%ax
        {
            return 1U;                                                               // 0x00401D1B: mov $0x1,%al
        }
        std::uint32_t const slot =
            (static_cast<std::uint32_t>(static_cast<std::uint16_t>(current)) * 3U) << 4U; // 0x00401CFE/0x00401D03/0x00401D06: and $0xffff; lea (%eax,%eax,2); shl $0x4
        std::uint16_t const parent =
            *reinterpret_cast<std::uint16_t const*>(type_table + slot + 0x08U);      // 0x00401D09: mov 0xaf5ee8(%eax),%ax
        if (parent == 0x0181U)                                                       // 0x00401D10/0x00401D14: cmp $0x181,%ax; je 0x401d1e
        {
            return 0U;                                                               // 0x00401D1E: xor %al,%al
        }
        current = parent;                                                            // 0x00401D19: jne 0x401cfe
    }
}                                                                                    // 0x00401D20: ret
} // extern "C"

#undef NATIVE_DECL
