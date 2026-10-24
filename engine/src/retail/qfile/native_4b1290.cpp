// Native: QFile_SetDeviceName_4B1290; retail VA 0x004B1290 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1290-0x004B12CC (61 bytes, 1 block, 0 edges, 24 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.2 of the evidence sheet (qfile device-driver table, name field at record+0x7C).
// Behavior/CFG: copies the NUL-terminated third argument into the 0x8C-stride device record's name buffer at 0x009436B0 + index*0x8C + 0x7C; the second argument is pushed by callers but never read. No branch, no call, no return value.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Table storage is owned by native_4b1180.cpp (g_9436b0). Retail inlines strlen and the dword/byte copy (repne scasb, rep movsd, rep movsb); the transcription calls std::strlen/std::memcpy, so the emitted body ends in CRT calls instead of inline loops - recorded, not smoothed. Like retail there is no length bound on the destination.
#include "igi/retail/qfile/driver_table.hpp"

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

void NATIVE_DECL QFile_SetDeviceName_4B1290(std::int32_t index, std::int32_t argument2, const char* source)
{
    (void)argument2;                                                        // 0x004B1290/0x004B1296: only [esp+4] and [esp+0xC] are read
    // 0x004B129A-0x004B12A9: lea eax,[ecx*8]; sub eax,ecx; lea eax,[eax+eax*4]; lea edx,[eax*4+0x0094372C]
    std::uint8_t* const name = &::g_9436b0[0] + 0x8C * static_cast<std::uintptr_t>(index) + 0x7CU;
    const std::size_t length = std::strlen(source) + 1U;                     // 0x004B12A3-0x004B12B6: repne scasb, not ecx
    std::memcpy(name, source, length);                                       // 0x004B12BC-0x004B12C8: rep movsd / rep movsb, NUL included
}

} // extern "C"

#undef NATIVE_DECL
