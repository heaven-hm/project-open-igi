// Native: MTP_AddLODVirModel_4CEBA0; retail VA 0x004CEBA0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004CEBA0-0x004CEC09
//   (106 bytes, 6 blocks, 41 instructions; per-block instruction counts 4 / 3 / 3 / 4 / 1 / 26 at 0x004CEBA0, 0x004CEBA9, 0x004CEBAD,
//   0x004CEBB9, 0x004CEBCB, 0x004CEBCD from af + afbj).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image. The capacity check is `cmp eax,0x258` at 0x004CEBB2; the
//   record addressing is `lea eax,[eax+eax*8]` then `lea edx,[eax*8+0x00A443A0]`, i.e. a 0x48-byte stride from 0x00A443A0; the name is copied to
//   record+0x14 (0x004CEBE4) by the inlined repne-scasb/rep-movsd strcpy at 0x004CEBDE-0x004CEBF7; the count is the dword at 0x00A4EDEC; the
//   only call is 0x004AF7B0 (Debug_Error, transcribed in core/retail_error.cpp). Domain: model/, not core/, because the only literal it can report
//   is "MTP_nLODVirModel >= %d" (0x00548210) - this routine registers MTP-side LOD virtual-model names, even though it is plain table code.
// Behavior/CFG: null name returns 0. Otherwise, if the record count at 0x00A4EDEC is at least 600 the routine reports
//   Debug_Error("MTP_nLODVirModel >= %d", 600) and spins forever at 0x004CEBCB; otherwise it takes record 0x00A443A0 + count*0x48, copies the name
//   including its terminator to record+0x14, increments the count and returns the record address. Branch conditions translated: name != null
//   (`test edi,edi; jne` at 0x004CEBA5/0x004CEBA7), count < 600 (`cmp eax,0x258; jl` at 0x004CEBB2/0x004CEBB7), and the post-report spin
//   (`jmp 0x004CEBCB` at 0x004CEBCB).
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The 16-byte name field at record+0x14 and the count at 0x00A4EDEC are established; the other
//   fields of the 0x48-byte record and the upper bound of the 600-record table's own extent are not, so the record stays a byte pointer. The
//   inlined strcpy is imported as std::strlen + std::memcpy (CRT declared, not re-implemented) - a declared behavioural-equivalent limitation,
//   not an instruction-equivalent copy. 0x00A4EDEC is not owned here: core/native_4cef30.cpp defines it as the one zero-initialised pair
//   g_a4edec_pair[2] covering 0x00A4EDEC and 0x00A4EDF0, and this file refers to element 0. The table 0x00A443A0 has no other owner.
#include "dev_log.hpp"
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

// Callee 0x004AF7B0 (Debug_Error), declared exactly as core/native_401900.cpp declares it.
std::int32_t NATIVE_DECL Debug_Error(const char* format, ...);

// 0x00A4EDEC/0x00A4EDF0: one zero-initialised pair, defined in core/native_4cef30.cpp; element 0 is the record count read and incremented here.
extern std::uint32_t g_a4edec_pair[2];

// 0x00A443A0: the LOD virtual-model record table. Capacity 600 comes from the retail bound (`cmp eax,0x258`); stride 0x48 comes from the
// record addressing. The name field is at +0x14.
std::uint8_t g_a443a0[600U * 0x48U];

std::uint8_t* NATIVE_DECL MTP_AddLODVirModel_4CEBA0(const char* name)
{
    NATIVE_LOG_ENTER("MTP_AddLODVirModel_4CEBA0", "0x004CEBA0");
    if (name == nullptr)                                    // 0x004CEBA1/0x004CEBA5: mov edi,[esp+8]; test edi,edi
    {
        return nullptr;                                     // 0x004CEBA7/0x004CEBA9: jne 0x004CEBAD; xor eax,eax
    }
    std::uint32_t const count = ::g_a4edec_pair[0];         // 0x004CEBAD: mov eax,[0xA4EDEC]
    if (count >= 600U)                                      // 0x004CEBB2/0x004CEBB7: cmp eax,0x258; jl 0x004CEBCD
    {
        Debug_Error("MTP_nLODVirModel >= %d", 0x258);       // 0x004CEBB9-0x004CEBC8: push 0x258; push "MTP_nLODVirModel >= %d"; call; add esp,8
        for (;;)                                            // 0x004CEBCB: jmp 0x004CEBCB (retail spins here)
        {
        }
    }
    std::uint8_t* const record = ::g_a443a0 + (count * 0x48U);              // 0x004CEBCD/0x004CEBD5: lea eax,[eax+eax*8]; lea edx,[eax*8+0xA443A0] (eax = the pre-branch count)
    std::memcpy(record + 0x14, name, std::strlen(name) + 1U);               // 0x004CEBDE-0x004CEBF7: inline strcpy to record+0x14, terminator included
    ::g_a4edec_pair[0] += 1U;                                               // 0x004CEBF9-0x004CEC01: mov eax,[0xA4EDEC]; inc eax; mov [0xA4EDEC],eax
    return record;                                                          // 0x004CEC06: mov eax,edx
}

} // extern "C"

#undef NATIVE_DECL
