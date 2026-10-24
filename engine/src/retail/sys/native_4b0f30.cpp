// Native: Sys_SelectRenderMode_4B0F30; retail VA 0x004B0F30 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0F30-0x004B0F61 (50 bytes, 3 blocks, 3 edges, 17 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes (8b442404 8b0d644aa900 3bc1 7423 8bc8 56 c1e106 03c8 57 bf804ea900 a3644aa900 8d34cda050a900 b982000000 f3a5 5f 5e c3); section 22.3 of the evidence sheet.
// Behavior/CFG: returns immediately when the requested mode equals the current mode word 0x00A94A64; otherwise records the mode in 0x00A94A64 and copies the 0x208-byte mode block at 0x00A94E80 into the 0x208-stride slot table at 0x00A950A0 + mode*0x208. No calls; no branch other than the equal-mode early return.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The slot table 0x00A950A0 is owned by sys/native_4b0e00.cpp and the mode word 0x00A94A64 by sys/native_4b0f70.cpp; the 0x208-byte source block 0x00A94E80 has no owner in the tree, so this file defines it - it ends exactly at 0x00A950A0, so the definition does not overlap the slot table. What the source block holds is not established, and nothing writes it yet.
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

// 0x00A950A0: 0x208-stride render-mode slot table, 0x1244 bytes (eight slots) - owned by sys/native_4b0e00.cpp.
extern std::uint8_t g_a950a0[0x1244];
// 0x00A94A64: current render-mode word - owned by sys/native_4b0f70.cpp.
extern std::int32_t g_a94a64;
// 0x00A94E80: 0x208-byte mode block copied into the selected slot. No other owner in engine/, so this file defines it.
std::uint8_t g_a94e80[0x208];

void NATIVE_DECL Sys_SelectRenderMode_4B0F30(std::int32_t mode)
{
    if (mode == ::g_a94a64)                                         // 0x004B0F34-0x004B0F3C: mov ecx,[0x00A94A64]; cmp eax,ecx; jz 0x004B0F61
    {
        return;
    }

    ::g_a94a64 = mode;                                              // 0x004B0F4C: mov dword [0x00A94A64],eax
    std::uint8_t* const destination =                               // 0x004B0F41-0x004B0F51: shl ecx,6; add ecx,eax; lea esi,[ecx*8+0x00A950A0]
        &::g_a950a0[0] + 0x208U * static_cast<std::uintptr_t>(mode);
    std::memcpy(destination, &::g_a94e80[0], 0x208U);               // 0x004B0F58-0x004B0F5D: mov ecx,0x82; rep movsd
}

} // extern "C"

#undef NATIVE_DECL
