// Native: Sys_ClearSlotByte_4B0EB0; retail VA 0x004B0EB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0EB0-0x004B0EBC (13 bytes, 1 block, 0 edges, 3 instructions: 8b442404 / c604c5844aa90000 / c3).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet (landability sweep row).
// Behavior/CFG: stores 0 into the flag byte at 0x00A94A84 + index*8, i.e. the +0x04 flag of the 8-byte record at 0x00A94A80 + index*8; no branch, no call, no stack use beyond the argument.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The table is defined by native_4b0e70.cpp (g_a94a80) as the 0x404-byte record table 0x004B0E70 registers slots in, so this file refers to that definition instead of defining a second overlapping array.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A94A80: the record table 0x004B0E70 registers slots in; this routine clears a record's +0x04 flag.
extern std::uint8_t g_a94a80[0x404];

void NATIVE_DECL Sys_ClearSlotByte_4B0EB0(std::int32_t index)
{
    ::g_a94a80[4U + static_cast<std::uintptr_t>(index) * 8U] = 0U;   // 0x004B0EB4: mov byte [eax*8 + 0x00A94A84], 0
}   // 0x004B0EBC: ret

} // extern "C"

#undef NATIVE_DECL
