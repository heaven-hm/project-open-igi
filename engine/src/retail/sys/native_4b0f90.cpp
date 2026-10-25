// Native: Sys_EnterRenderMode_4B0F90; retail VA 0x004B0F90 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0F90-0x004B0FC4 (53 bytes, 3 blocks, 2 edges, 19 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes (a104239400 56 8b348508239400 83c8ff 3bf0 741a 8b1504239400 56 42 891504239400 e879ffffff 83c404 8bc6 5e c3 a3644aa900 5e c3); section 22.3 of the evidence sheet.
// Behavior/CFG: reads the cursor word 0x00942304, loads the entry at 0x00942308 + cursor*4, and when that entry is -1 writes -1 into the current render-mode word 0x00A94A64 and returns -1 without advancing the cursor; otherwise advances the cursor word by one, activates the entry through Sys_SelectRenderMode (0x004B0F30), and returns it.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. 0x00942304 is defined by sys/native_4b0dc0.cpp and referred to extern here. The cursor table 0x00942308 has no other reference in the tree; its extent is bounded by the next known object, the alias record table at 0x00942330, so 0x00942330 - 0x00942308 = 0x28 bytes = 10 dwords and this file defines it. Retail performs no range check on the cursor, so a deep enough nesting reads past that array - the same overrun is preserved. What the entries hold before any leave operation is not established by the bytes.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00942304: the mode-stack cursor (owned by sys/native_4b0dc0.cpp).
extern std::int32_t g_942304;
// 0x00942308: the mode stack this routine pops from and 0x004B0FD0 pushes to. No other owner in the tree.
std::int32_t g_942308[10];
// 0x00A94A64: the current render-mode word (owned by sys/native_4b0f70.cpp).
extern std::int32_t g_a94a64;

// 0x004B0F30 (sys/native_4b0f30.cpp): records a mode word and copies its 0x208-byte block into the slot table.
void NATIVE_DECL Sys_SelectRenderMode_4B0F30(std::int32_t mode);

std::int32_t NATIVE_DECL Sys_EnterRenderMode_4B0F90(void)
{
    const std::int32_t cursor = ::g_942304;                            // 0x004B0F90: mov eax,[0x00942304]
    const std::int32_t mode = ::g_942308[static_cast<std::uintptr_t>(cursor)]; // 0x004B0F96: mov esi,[eax*4+0x00942308]
    if (mode == -1)                                                    // 0x004B0F9D-0x004B0FA2: or eax,-1; cmp esi,eax; jz 0x004B0FBE
    {
        ::g_a94a64 = -1;                                               // 0x004B0FBE: mov [0x00A94A64],eax
        return -1;                                                     // 0x004B0FC3-0x004B0FC4: pop esi; ret
    }

    ::g_942304 = cursor + 1;                                           // 0x004B0FA4-0x004B0FAF: mov edx,[0x00942304]; inc edx; mov [0x00942304],edx
    Sys_SelectRenderMode_4B0F30(mode);                                 // 0x004B0FB2: call 0x004B0F30
    return mode;                                                       // 0x004B0FBA: mov eax,esi
}

} // extern "C"

#undef NATIVE_DECL
