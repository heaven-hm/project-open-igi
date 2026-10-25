// Native: Sys_ClearRenderModeSlot_4B0E50; retail VA 0x004B0E50 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B0E50-0x004B0E63 (20 bytes, 1 block, 0 edges, 7 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes (8b442404 8bc8 c1e106 03c8 c604cda452a90000 c3); section 22.3 of the evidence sheet.
// Behavior/CFG: clears the in-use flag byte of one render-mode slot: 0x00A952A4 + index*0x208, which is the +0x204 flag of the record at 0x00A950A0 + index*0x208. Retail computes the stride as (index<<6 + index)<<3. No branch, no call, no return value, no bound check.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The table is owned by sys/native_4b0e00.cpp (g_a950a0); this file refers to it. There is no index bound, matching retail.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A950A0: render-mode record table owned by sys/native_4b0e00.cpp; 0x208 stride, flag at +0x204.
extern std::uint8_t g_a950a0[0x1244];

void NATIVE_DECL Sys_ClearRenderModeSlot_4B0E50(std::int32_t index)
{
    NATIVE_LOG_ENTER("Sys_ClearRenderModeSlot_4B0E50", "0x004B0E50");
    // 0x004B0E54-0x004B0E5B: ecx = index<<6; ecx += index; then movb bash, 0x00A952A4(,%ecx,8) -> flag = index*0x208 + 0x204.
    ::g_a950a0[0x204U + static_cast<std::uintptr_t>(index) * 0x208U] = 0U;
}   // 0x004B0E63: ret

} // extern "C"

#undef NATIVE_DECL
