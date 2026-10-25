// Native: Display_ApplyFlags_491980 (placeholder until VERIFIED); retail VA 0x00491980-0x00491A22 (163 bytes, 21 blocks, 32 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) instruction listing 0x00491980-0x00491A22 (51 a18403c100 55 33c9 33ed 85c0 894c2404 0f8e87000000 ... 891d 8403c100 in the builder 0x00492670); radare2 6.2.0 af/afij (163 B, 21 blocks, 68 instructions, 32 edges) and afbj for the per-block counts; the table shape is read from its only writer, the untranscribed builder 0x00492670 (`lea 0x0(%ebp,%ebp,8),%edx; shl $0x3,%edx; sub %ebp,%edx; shl $0x2,%edx` = index*0x11C, `mov $0x47,%ecx; rep stos` = 284 zeroed bytes, `cmp $0x3e8,%ebp; jge` = the 1000-record bound, and the stores at 0x00BCAE24/28/30/38); the second call site 0x004909BC pushes three arguments.
// Behavior/CFG: walks the count at 0x00C10384 records of the table at 0x00BCAE20 (the cursor 0x00BCAE28 is eight bytes into the record, so the compared dwords are the record's +0x04/+0x08/+0x10/+0x18), and for each record compares those fields against the key in argument1 under the flag byte of argument2 (bit 1 -> +0x04, 2 -> +0x08, 4 -> +0x10, 8 -> +0x18; a null key matches every record). Every matching record invokes argument3 with the record address and increments the return counter; the count at 0x00C10384 is re-read on every iteration, so a callback that changes it changes the loop bound.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 68-instruction listing and the 21-block/32-edge CFG; runtime UNOBSERVED. The third argument is read at 0x004919EC as `mov 0x20(%esp),%eax` - with the three register saves on the stack that operand is the third argument word [E+0x0C], and the raw offset 0x20 is what the image encodes: the call site 0x004909BC pushes it, but the 0x00402FF0 call site pushes only two arguments, so retail's third argument there is a stale stack word - that routine's saved EDI slot from 0x00402FF7, a value not knowable statically; the callee's own null check at 0x004919F2 skips it, and the port call site passes null and records the difference there. The table and count are declared here with their image values (count 0) because the image's only writer is the untranscribed 0x00492670, which must reuse these definitions when it lands (one definition per VA). Retail's own layout overlaps them: the 1000-record bound spans 0x00BCAE20-0x00C103E0, so the count dword 0x00C10384 sits inside the last records; the port declares two separate objects, which does not affect this body, whose only use of the count is the loop bound.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// Retail 0x00C10384: records currently in the table below (image value 0). Written by the builder 0x00492670 and by 0x004917C0.
std::int32_t g_c10384 = 0;

// Retail 0x00BCAE20: the record table, 0x3E8 records of 0x11C bytes - the bound the builder enforces at 0x00492677.
std::uint8_t g_bcae20[0x3E8 * 0x11C] = {};

std::int32_t NATIVE_DECL Display_ApplyFlags_491980(void* const argument1, std::int32_t const argument2,
                                                    void* const argument3)
{
    NATIVE_LOG_ENTER("Display_ApplyFlags_491980", "0x00491980");
    std::int32_t matches = 0;                          // 0x00491987: xor %ecx,%ecx (retail keeps it in the pushed ecx slot at frame -0x04)
    if (g_c10384 <= 0)                                 // 0x00491981/0x0049198B/0x00491991: mov 0xc10384,%eax; test %eax,%eax; jle 0x00491A1E
    {
        return matches;                                // 0x00491A1E: mov %ecx,%eax
    }

    std::uint8_t const flags = static_cast<std::uint8_t>(argument2);            // 0x00491998: mov 0x14(%esp),%bl
    std::uint8_t const* const key = static_cast<std::uint8_t const*>(argument1); // 0x0049199E: mov 0x18(%esp),%edi
    std::uint8_t* record = g_bcae20;                                            // 0x004919A2: mov $0xbcae28,%esi (retail's cursor is eight bytes into the record)
    std::int32_t index = 0;                                                     // 0x00491989: xor %ebp,%ebp

    do
    {
        std::int32_t matched = 1;                      // 0x004919A9: mov $0x1,%eax
        if (key != nullptr)                            // 0x004919A7/0x004919AE: test %edi,%edi; je 0x00491EC
        {
            if ((flags & 0x1U) != 0)                   // 0x004919B0/0x004919B2: test %bl,%al; je 0x004919BE (al holds the 1 just loaded into eax)
            {
                if (*reinterpret_cast<std::uint32_t const*>(key + 0x04) !=
                    *reinterpret_cast<std::uint32_t const*>(record + 0x04))
                {
                    matched = 0;                       // 0x004919BC: xor %eax,%eax
                }
            }
            if ((flags & 0x2U) != 0)                   // 0x004919BE/0x004919C1: test $0x2,%bl; je 0x004919CC
            {
                if (*reinterpret_cast<std::uint32_t const*>(key + 0x08) !=
                    *reinterpret_cast<std::uint32_t const*>(record + 0x08))
                {
                    matched = 0;                       // 0x004919CA: xor %eax,%eax
                }
            }
            if ((flags & 0x4U) != 0)                   // 0x004919CC/0x004919CF: test $0x4,%bl; je 0x004919DB
            {
                if (*reinterpret_cast<std::uint32_t const*>(key + 0x10) !=
                    *reinterpret_cast<std::uint32_t const*>(record + 0x10))
                {
                    matched = 0;                       // 0x004919D9: xor %eax,%eax
                }
            }
            if ((flags & 0x8U) != 0)                   // 0x004919DB/0x004919DE: test $0x8,%bl; je 0x004919E8
            {
                if (*reinterpret_cast<std::uint32_t const*>(key + 0x18) !=
                    *reinterpret_cast<std::uint32_t const*>(record + 0x18))
                {
                    goto next_record;                  // 0x004919E6: jne 0x00491A06
                }
            }
            if (matched == 0)                          // 0x004919E8/0x004919EA: test %eax,%eax; je 0x00491A06
            {
                goto next_record;
            }
        }

        if (argument3 != nullptr)                      // 0x004919EC/0x004919F2: mov 0x20(%esp),%eax; test %eax,%eax; je 0x00491A01
        {
            reinterpret_cast<void (*)(void*)>(argument3)(record); // 0x004919F4/0x004919F7/0x004919F8: lea -0x8(%esi),%ecx; push %ecx; call *%eax
        }
        ++matches;                                     // 0x00491A01: inc %ecx

    next_record:
        index += 1;                                    // 0x00491A0B: inc %ebp
        record += 0x11C;                               // 0x00491A0C: add $0x11c,%esi
    } while (index < g_c10384);                        // 0x00491A06/0x00491A12/0x00491A14: mov 0xc10384,%eax; cmp %eax,%ebp; jl 0x004919A7

    return matches;                                    // 0x00491A16-0x00491A1D: pop edi/esi/ebx; mov %ecx,%eax; pop ebp/ecx; ret
}

} // extern "C"

#undef NATIVE_DECL
