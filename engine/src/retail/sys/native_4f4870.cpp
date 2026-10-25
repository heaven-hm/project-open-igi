// Native: Sys_SetGlobalA76C40_4F4870 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004F4870.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F4870-0x004F4879 (10 bytes, 1 block, 0 edges, 3 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf plus the raw image bytes read at the VA through the PE section table (8b442404 a3406ca700 c3):
//   `mov eax,[esp+4]` at 0x004F4870, `mov [0x00A76C40],eax` at 0x004F4874, `ret` at 0x004F4879. 0x004F487A-0x004F487F are six nops and 0x004F4880
//   starts the next function, so the extent above is the whole routine. 0x00A76C40 has exactly three accesses in the whole image and all three
//   are dword-sized: this store and the two loads at 0x004F4560 and 0x004F457F (both `mov eax,dword ptr [0xa76c40]`), which is the width
//   declared below. The GameNatives catalogue's candidate name for this VA, `Script_CallNativeHandler_17`, is contradicted by the body and is
//   not used: this routine takes one argument and stores it, it calls nothing, and it cannot be a native-handler dispatcher.
// Behavior/CFG: one straight-line body, no frame of its own, no saved register, no branch, no loop, no call, no local state. It stores its
//   single dword argument into 0x00A76C40 and returns. 0x00A76C40 holds image value 0 - it is above the file-backed end of .data at
//   0x00550000, so its initialiser is BSS-zero - and this routine is its only writer in the image.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. One argument, read before any frame exists, at [esp+4]. Declared void because the body never
//   writes eax: retail leaves the argument in eax at the ret, so a caller-visible value there is the caller's own register, not a result this
//   routine computes, and no caller in the image reads it. The value's meaning is not established by this body; both readers (0x004F4560 and
//   0x004F457F) sit inside untranscribed functions, so nothing here proves what the slot is for. Domain: sys/, the same folder as the sibling
//   single-slot accessors 0x004F45F0 and 0x004F4850.
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

// 0x00A76C40: the dword slot this setter writes; nothing else in the tree owns it yet.
std::uint32_t g_a76c40;

void NATIVE_DECL Sys_SetGlobalA76C40_4F4870(std::uint32_t value)
{
    NATIVE_LOG_ENTER("Sys_SetGlobalA76C40_4F4870", "0x004F4870");
    ::g_a76c40 = value;                              // 0x004F4874: mov [0x00A76C40],eax; 0x004F4879: ret
}

} // extern "C"

#undef NATIVE_DECL
