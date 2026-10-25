// Native: Sys_SetSlotWord_4B0EC0; retail VA 0x004B0EC0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0EC0-0x004B0EDD (30 bytes, 1 block, 0 edges, 9 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet (landability sweep row).
// Behavior/CFG: computes (first<<6 + first)*2 + second, i.e. first*0x82 + second, and stores the third argument into the dword table at 0x00A950A4 indexed by that value; no branch, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The 0x00A950A4 table has no other owner in the tree, so this file defines it.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A950A4: the dword table this routine writes; 130 entries per first-index block.
std::uint32_t g_a950a4[0x2000];

void NATIVE_DECL Sys_SetSlotWord_4B0EC0(std::int32_t first, std::int32_t second, std::uint32_t value)
{
    const std::uint32_t index = static_cast<std::uint32_t>(second) +
                                static_cast<std::uint32_t>(first) * 0x82U;   // 0x004B0EC8-0x004B0ECF: (first<<6 + first)*2 + second
    ::g_a950a4[index] = value;                                              // 0x004B0ED6: mov dword [eax*4 + 0x00A950A4], ecx
}   // 0x004B0EDD: ret

} // extern "C"

#undef NATIVE_DECL
