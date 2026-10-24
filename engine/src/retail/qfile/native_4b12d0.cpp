// Native: QFile_ClearTable_4B12D0; retail VA 0x004B12D0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B12D0-0x004B12E7 (24 bytes, 3 blocks, 3 edges).
// Tools/evidence: radare2 6.2.0 af/pdf; section 22 of the evidence sheet (qfile init chain).
// Behavior/CFG: walks from 0x009423C0 in 0x94-byte steps while below 0x00943640, storing zero to the dword at each step; returns void. Nothing else is read or written.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. The 0x94-stride block from 0x009423C0 is BSS and has no owner yet, so it is declared here.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x009423C0: the 0x94-stride table this routine clears. 0x00943640 - 0x009423C0 = 0x1280 bytes, so 0x1280 / 0x94
// iterations; the loop condition is on the pointer, exactly as retail compares against the immediate.
std::uint8_t g_9423c0[0x1280];

void NATIVE_DECL QFile_ClearTable_4B12D0(void)
{
    std::uint8_t* entry = &::g_9423c0[0];                            // 0x004B12D0: mov eax, 0x009423C0
    do
    {
        *reinterpret_cast<std::uint32_t*>(entry) = 0U;               // 0x004B12D5: mov dword [eax], 0
        entry += 0x94U;                                              // 0x004B12DB: add eax, 0x94
    } while (reinterpret_cast<std::uintptr_t>(entry) <
             reinterpret_cast<std::uintptr_t>(&::g_9423c0[0]) + 0x1280U); // 0x004B12E0: cmp eax, 0x00943640 / jl
}                                                                    // 0x004B12E7: ret

} // extern "C"

#undef NATIVE_DECL
