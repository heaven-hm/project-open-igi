// Native: Sys_LookupOrAppendRecord_416930; retail VA 0x00416930-0x004169C3 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00416930-0x004169C3
//   (148 bytes, 13 blocks, 66 instructions, measured per block with af + afbj over the exact image).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf plus a pd 12 read of the two blocks af/pdf display out of order (0x00416980-0x0041698D and the
//   found-return epilogue); the count dword at 0x0057BAB4, the table base 0x0057B5E8 and the 0x3C record stride are the mov/lea immediates
//   in the body itself, and 0x00416984-0x0041698D is `lea eax,[ebp+ebp*2]; lea ecx,[eax+eax*4]; lea edx,[ecx*4+0x0057B5E8]`, i.e. an
//   index * 15 * 4 = index * 0x3C destination computation.
// Behavior/CFG: searches the record table at 0x0057B5E8 for the record whose leading NUL-terminated name matches the argument, walking the
//   table with the 0x3C stride while the index is below the dword count at 0x0057BAB4 (13 blocks: prologue/count test, table base, loop head,
//   the two-byte-unrolled MSVC inline strcmp expansion and its five exits, the increment, the append block and the two return epilogues).
//   On a match it returns the record address; otherwise it appends a record at index = count, copies the argument string INCLUDING its
//   terminator into it with retail's inline strlen (repne scasb) plus inline memcpy (rep movsd then rep movsb), increments the count, and
//   returns the new record address. The rest of an appended record is not initialized by this routine - retail copies the name only.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The comparison is transcribed block by block, but its differing-byte result is retail's
//   `sbb ecx,ecx; sbb ecx,-1` sign trick whose magnitude is never observed (the only consumer is `test ecx,ecx`), so this transcription keeps
//   Retail's inline strlen (repne scasb) and inline memcpy (rep movsd/rep movsb) are expressed with the CRT routines of the same name, which
//   are imported rather than re-implemented; the byte count and the terminator handling are retail's (strlen + 1).
//   the equality outcome and documents that the +1/-1 magnitude is not reproduced. The table's capacity is not established by static evidence,
//   which is why the table is addressed by its absolute retail address instead of a declared array extent. The name string's width is the
//   callers' literal (0x0047E700 passes "burn1.tex"); the record fields after the name are untouched here.
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

// 0x0057BAB4: the record count this routine reads and increments (image value 0). The table at 0x0057B5E8 has no statically established
// extent, so it is reached through its absolute address rather than through an invented array bound.
std::int32_t g_57bab4 = 0;

void* NATIVE_DECL Sys_LookupOrAppendRecord_416930(char const* name)
{
    std::int32_t const count = ::g_57bab4;                                                   // 0x00416931: mov ebx,dword [0x57BAB4]
    std::uint8_t* record = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(0x0057B5E8U)); // 0x00416940: mov edi,0x57B5E8
    std::int32_t index = 0;                                                                  // 0x00416938: xor ebp,ebp
    if (count > 0)                                                                           // 0x0041693C/0x0041693E: test ebx,ebx; jle
    {
        for (;;)                                                                             // 0x00416945: loop head
        {
            std::uint8_t const* lhs = record;                                                // 0x00416949: mov ecx,edi
            std::uint8_t const* rhs = reinterpret_cast<std::uint8_t const*>(name);            // 0x00416945: mov esi,[esp+0x14]
            std::int32_t differs;
            for (;;)                                                                         // 0x0041694B-0x00416969: MSVC inline strcmp, two bytes per step
            {
                std::uint8_t const first = lhs[0];                                            // 0x0041694B: mov al,[ecx]
                if (first != rhs[0]) { differs = 1; break; }                                  // 0x0041694F/0x00416951: cmp al,[esi]; jne
                if (first == 0U) { differs = 0; break; }                                       // 0x00416953/0x00416955: test dl,dl; je
                std::uint8_t const second = lhs[1];                                            // 0x00416957: mov al,[ecx+1]
                if (second != rhs[1]) { differs = 1; break; }                                 // 0x0041695C/0x0041695F: cmp al,[esi+1]; jne
                lhs += 2;                                                                      // 0x00416961: add ecx,2
                rhs += 2;                                                                      // 0x00416964: add esi,2
                if (second == 0U) { differs = 0; break; }                                      // 0x00416967/0x00416969: test dl,dl; jne loop
            }
            // 0x0041696B: xor ecx,ecx (equal) / 0x0041696F: sbb ecx,ecx; sbb ecx,-1 (differ); 0x00416974: test ecx,ecx.
            if (differs == 0) { return record; }                                              // 0x00416976: je 0x004169BD
            ++index;                                                                          // 0x00416978: inc ebp
            record += 0x3C;                                                                    // 0x00416979: add edi,0x3C
            if (index >= count) { break; }                                                    // 0x0041697C/0x0041697E: cmp ebp,ebx; jl
        }
    }

    // 0x00416980: not found - append at index = count.
    std::uint8_t* const destination = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(0x0057B5E8U)) +
                                      static_cast<std::size_t>(index) * 0x3CU;             // 0x00416984-0x0041698D: three leas
    std::uint8_t const* const source = reinterpret_cast<std::uint8_t const*>(name);            // 0x00416980: mov edi,[esp+0x14]
    // Retail inlines strlen as `repne scasb; not ecx; sub edi,ecx` and then copies strlen+1 bytes (terminator included) with `rep movsd`
    // over `rep movsb`. The CRT routines below are the same operations; they are imported, not re-implemented.
    std::size_t const length = std::strlen(reinterpret_cast<char const*>(source)) + 1U;
    std::memcpy(destination, source, length);
    ++::g_57bab4;                                                                              // 0x004169AF: inc ebx
    return destination;                                                                        // 0x004169AA: mov eax,edx
    // 0x004169B4: mov dword [0x57BAB4],ebx; 0x004169BD: found-return `mov eax,edi`; both epilogues pop ebx/ebp/esi/edi and ret.
}

} // extern "C"

#undef NATIVE_DECL
