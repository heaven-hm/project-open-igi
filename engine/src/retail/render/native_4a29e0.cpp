// Native: Render_StoreArgs936024_4A29E0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004A29E0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004A29E0-0x004A29FD (30 bytes, 1 block, 0 edges, 7 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf over the exact image, plus the raw bytes read at the VA (8b442404 8b4c2408 8b54240c a324609300 890d30609300 891534609300 c3):
//   three dword loads of the three arguments at 0x004A29E0/0x004A29E4/0x004A29E8, then `mov [0x00936024],eax` at 0x004A29EC, `mov [0x00936030],ecx` at
//   0x004A29F1, `mov [0x00936034],edx` at 0x004A29F7 and `ret` at 0x004A29FD. 0x004A29FE-0x004A29FF are two nops and 0x004A2A00 starts the next function
//   (`push 0; push 0x00543E38 "CAMERAFILTER_TYPE_NONE"`), so the extent above is the whole routine. The only caller in the image is 0x00415530 Game_Init,
//   which calls it at 0x00415799 after pushing 0x00539588 ("LOCAL:computer/font1.fnt"), 0x75 and 0x14, so the three slots receive the address of that
//   literal, 0x75 and 0x14 in that order. Each of the three slots has exactly one reader besides this store, and their instruction forms fix the width:
//   0x00936024 is read at 0x004A2969 with `mov eax,dword ptr [0x936024]` (a dword, consistent with the literal address stored here), while 0x00936030
//   is read at 0x004A4DFE with `fisub dword ptr [0x936030]` and 0x00936034 at 0x004A4DE2 with `fisub dword ptr [0x936034]` - the `fi` prefix is the
//   x87 INTEGER subtract, so those two slots hold 32-bit integers and not floats. All three are declared as dwords below, which is the width every
//   access proves.
// Behavior/CFG: one straight-line body, no frame of its own, no saved register, no branch, no loop, no call, no local state. It stores its three
//   dword arguments into 0x00936024, 0x00936030 and 0x00936034, in that order, and returns.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Three arguments, read before any frame exists, at [esp+4], [esp+8] and [esp+0xc]. Declared void
//   because the body never writes eax after the loads: retail leaves the last loaded argument in eax at the ret, so a caller-visible value there is
//   the caller's own register, not a result this routine computes, and the only caller discards it. The three slots hold image value 0 (they are above
//   the file-backed end of .data at 0x00550000) and this routine is their only writer. The meaning of the slots is not established by this body; the
//   two `fisub` readers sit inside untranscribed functions. Domain: render/, the camera module that 0x004A2A00 and the sibling setters 0x004A29C0 and
//   0x004A28B0 belong to.
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

// 0x00936024: holds the literal address the only caller passes; 0x00936030 and 0x00936034: the two integer slots its `fisub` readers use.
// Nothing else in the tree owns any of the three yet.
std::uint32_t g_936024;
std::uint32_t g_936030;
std::uint32_t g_936034;

void NATIVE_DECL Render_StoreArgs936024_4A29E0(std::uint32_t arg1, std::uint32_t arg2, std::uint32_t arg3)
{
    NATIVE_LOG_ENTER("Render_StoreArgs936024_4A29E0", "0x004A29E0");
    ::g_936024 = arg1;                               // 0x004A29EC: mov [0x00936024],eax
    ::g_936030 = arg2;                               // 0x004A29F1: mov [0x00936030],ecx
    ::g_936034 = arg3;                               // 0x004A29F7: mov [0x00936034],edx; 0x004A29FD: ret
}

} // extern "C"

#undef NATIVE_DECL
