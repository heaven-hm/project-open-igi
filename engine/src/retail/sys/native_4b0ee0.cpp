// Native: Sys_EnumRenderModes_4B0EE0; retail VA 0x004B0EE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0EE0-0x004B0F2B (76 bytes, 5 blocks, 6 edges, 26 instructions).
// Tools/evidence: radare2 6.2.0 af/pd and the raw bytes (83ec08 53 8b5c2414 55 8b6c2414 56 57 33ff bea050a900 / 8a8604020000 ... / ffd5 / 81c608020000 47 81fee060a900 / c3); section 22.3 of the evidence sheet.
// Behavior/CFG: walks the eight 0x208-stride render-mode records at 0x00A950A0 up to 0x00A960E0 and, for each record whose +0x204 flag is non-zero, calls the first argument as a cdecl callback with (pointer to a two-word local {index, record name pointer}, second argument). The locals are filled AFTER the two pushes in retail's own order: index first, then the name pointer. Nothing is returned.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The record table 0x00A950A0 is defined by native_4b0e00.cpp; this file refers to it. The callback's own contract is unproven beyond the observed two-argument cdecl call shape, and the eight-record count is the 0x1040-byte scanned extent divided by 0x208.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A950A0: render-mode record table owned by native_4b0e00.cpp; 0x208 stride, flag at +0x204.
extern std::uint8_t g_a950a0[0x1244];

// The two-word local retail builds for the callback: index at +0x00, record name pointer at +0x04.
struct RenderModeEntry {
    std::uint32_t index;
    const char* name;
};

void NATIVE_DECL Sys_EnumRenderModes_4B0EE0(void (NATIVE_DECL* callback)(RenderModeEntry*, std::int32_t),
                                            std::int32_t argument)
{
    std::uint8_t* record = &::g_a950a0[0];                 // 0x004B0EF1: esi = 0x00A950A0
    std::uint32_t index = 0;                               // 0x004B0EEF: edi = 0

    for (;;)
    {
        if (record[0x204U] != 0U)                          // 0x004B0EF6-0x004B0EFE: mov al,[esi+0x204]; test al,al; je 0x004B0F15
        {
            RenderModeEntry entry;
            entry.index = index;                           // 0x004B0F08: mov [esp+0x18], edi
            entry.name = *reinterpret_cast<const char**>(record); // 0x004B0F00/0x004B0F0C: mov eax,[esi]; mov [esp+0x1c], eax
            // 0x004B0F06-0x004B0F12: push argument; push &entry; call callback; add esp,8.
            callback(&entry, argument);
        }
        record += 0x208U;                                  // 0x004B0F15: add esi,0x208
        ++index;                                           // 0x004B0F1B: inc edi
        if (record >= &::g_a950a0[0] + 0x1044U)            // 0x004B0F1C-0x004B0F22: cmp esi,0x00A960E0; jl
        {
            return;                                        // 0x004B0F24-0x004B0F2B: pops; add esp,8; ret
        }
    }
}

} // extern "C"

#undef NATIVE_DECL
