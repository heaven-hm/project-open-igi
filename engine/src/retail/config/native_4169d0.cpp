// Native: Sys_LookupOrAppendRecord_4169D0; retail VA 0x004169D0-0x00416A62 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004169D0-0x00416A62
//   (147 bytes, 13 blocks, 66 instructions, measured per block with af + afbj over the exact image).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf plus a pd 12 read of the append block (0x00416A20-0x00416A62), which af/pdf split across its
//   block listing; the count dword at 0x0057BAB8, the table base 0x0057B1A8 and the 0x30 record stride are the mov/lea/shl immediates in the
//   body itself - 0x00416A24-0x00416A30 is `lea edx,[ebp+ebp*2]; shl edx,4; add edx,0x0057B1A8`, i.e. an index * 3 * 16 = index * 0x30
//   destination computation.
// Behavior/CFG: same shape as the sibling lookup at 0x00416930 but over a different table: it searches the records at 0x0057B1A8, stride 0x30,
//   count dword at 0x0057BAB8, comparing each record's leading NUL-terminated name with the argument using retail's two-byte-unrolled MSVC
//   inline strcmp (13 blocks: prologue/count test, table base, loop head, the unrolled compare and its exits, the increment, the append block
//   and the two return epilogues). On a match it returns the record address; otherwise it appends at index = count, copies the argument string
//   including its terminator with retail's inline strlen (repne scasb) and inline memcpy (rep movsd then rep movsb), increments the count and
//   returns the new record address. The rest of an appended record is not initialized here.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The comparison's differing-byte magnitude is retail's `sbb ecx,ecx; sbb ecx,-1` sign trick
//   and is never observed (only `test ecx,ecx` consumes it), so this transcription keeps the equality outcome and documents that the +1/-1
//   Retail's inline strlen (repne scasb) and inline memcpy (rep movsd/rep movsb) are expressed with the CRT routines of the same name, which
//   are imported rather than re-implemented; the byte count and the terminator handling are retail's (strlen + 1).
//   magnitude is not reproduced. The table capacity is not established by static evidence, so the table is addressed by its absolute retail
//   address instead of a declared array extent. What the record holds after its name is not established by this body (callers store the
//   returned pointer in a dword, e.g. 0x0047E700 keeps it at 0x005C114C).
#include <cstddef>
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0057BAB8: the record count this routine reads and increments (image value 0). The table at 0x0057B1A8 has no statically established
// extent, so it is reached through its absolute address rather than through an invented array bound.
std::int32_t g_57bab8 = 0;

void* NATIVE_DECL Sys_LookupOrAppendRecord_4169D0(char const* name)
{
    std::int32_t const count = ::g_57bab8;                                                   // 0x004169D1: mov ebx,dword [0x57BAB8]
    std::uint8_t* record = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(0x0057B1A8U)); // 0x004169E0: mov edi,0x57B1A8
    std::int32_t index = 0;                                                                  // 0x004169D8: xor ebp,ebp
    if (count > 0)                                                                           // 0x004169DC/0x004169DE: test ebx,ebx; jle
    {
        for (;;)                                                                             // 0x004169E5: loop head
        {
            std::uint8_t const* lhs = record;                                                // 0x004169E9: mov ecx,edi
            std::uint8_t const* rhs = reinterpret_cast<std::uint8_t const*>(name);            // 0x004169E5: mov esi,[esp+0x14]
            std::int32_t differs;
            for (;;)                                                                         // 0x004169EB-0x00416A09: MSVC inline strcmp, two bytes per step
            {
                std::uint8_t const first = lhs[0];                                            // 0x004169EB: mov al,[ecx]
                if (first != rhs[0]) { differs = 1; break; }                                  // 0x004169EF/0x004169F1: cmp al,[esi]; jne
                if (first == 0U) { differs = 0; break; }                                       // 0x004169F3/0x004169F5: test dl,dl; je
                std::uint8_t const second = lhs[1];                                            // 0x004169F7: mov al,[ecx+1]
                if (second != rhs[1]) { differs = 1; break; }                                 // 0x004169FC/0x004169FF: cmp al,[esi+1]; jne
                lhs += 2;                                                                      // 0x00416A01: add ecx,2
                rhs += 2;                                                                      // 0x00416A04: add esi,2
                if (second == 0U) { differs = 0; break; }                                      // 0x00416A07/0x00416A09: test dl,dl; jne loop
            }
            // 0x00416A0B: xor ecx,ecx (equal) / 0x00416A0F: sbb ecx,ecx; sbb ecx,-1 (differ); 0x00416A14: test ecx,ecx.
            if (differs == 0) { return record; }                                              // 0x00416A16: je 0x00416A5C
            ++index;                                                                          // 0x00416A18: inc ebp
            record += 0x30;                                                                    // 0x00416A19: add edi,0x30
            if (index >= count) { break; }                                                    // 0x00416A1C/0x00416A1E: cmp ebp,ebx; jl
        }
    }

    // 0x00416A20: not found - append at index = count.
    std::uint8_t* const destination = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(0x0057B1A8U)) +
                                      static_cast<std::size_t>(index) * 0x30U;             // 0x00416A24-0x00416A30: lea, shl, add
    std::uint8_t const* const source = reinterpret_cast<std::uint8_t const*>(name);            // 0x00416A20: mov edi,[esp+0x14]
    // Retail inlines strlen as `repne scasb; not ecx; sub edi,ecx` and then copies strlen+1 bytes (terminator included) with `rep movsd`
    // over `rep movsb`. The CRT routines below are the same operations; they are imported, not re-implemented.
    std::size_t const length = std::strlen(reinterpret_cast<char const*>(source)) + 1U;
    std::memcpy(destination, source, length);
    ++::g_57bab8;                                                                              // 0x00416A4E: inc ebx
    return destination;                                                                        // 0x00416A49: mov eax,edx
    // 0x00416A53: mov dword [0x57BAB8],ebx; 0x00416A5C: found-return `mov eax,edi`; both epilogues pop edi/esi/ebp/ebx and ret.
}

} // extern "C"

#undef NATIVE_DECL
