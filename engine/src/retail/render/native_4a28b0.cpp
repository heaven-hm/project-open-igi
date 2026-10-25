// Native: Render_StoreArgs935C04_4A28B0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004A28B0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004A28B0-0x004A28E0 (49 bytes, 1 block, 0 edges, 11 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf over the exact image, plus the raw bytes read at the VA (8b442404 8b4c2408 8b54240c a3045c9300 8b442410 890d3c609300 8b4c2414 891540609300 a344609300 890d48609300 c3).
//   Retail order: three dword loads of the first three arguments at 0x004A28B0/0x004A28B4/0x004A28B8, `mov [0x00935C04],eax` at 0x004A28BC, then the
//   load of the fourth argument at 0x004A28C1, `mov [0x0093603C],ecx` at 0x004A28C5, the load of the fifth argument at 0x004A28CB, `mov
//   [0x00936040],edx` at 0x004A28CF, `mov [0x00936044],eax` at 0x004A28D5, `mov [0x00936048],ecx` at 0x004A28DA and `ret` at 0x004A28E0. The two
//   loads at 0x004A28C1 and 0x004A28CB sit between the stores in retail and are the fetches of arguments four and five; they write no state, so the
//   body below keeps the five stores in retail order and nothing else. 0x004A28E1 onwards is nop padding. The only caller in the image is 0x00415530
//   Game_Init, which calls it at 0x004157C5 after pushing 0x00539588 ("LOCAL:computer/font1.fnt"), 0x3F666666, 0x3E4CCCCD, 0xC2480000 and 0, so the five
//   slots receive the address of that literal, 0.9f, 0.2f, -50.0f and 0 in that order. Each of the five slots has exactly one reader besides this store,
//   and their instruction forms fix the width and the kind: 0x00935C04 is read at 0x004A25BB with `mov eax,dword ptr [0x935c04]` (a dword, consistent
//   with the literal address stored here), while 0x0093603C is read at 0x004A375D with `fmul dword ptr [0x93603c]`, 0x00936040 at 0x004A3736 with `fmul
//   dword ptr [0x936040]`, 0x00936044 at 0x004A3764 with `fadd dword ptr [0x936044]` and 0x00936048 at 0x004A3746 with `fadd dword ptr [0x936048]` - the
//   x87 FLOATING-POINT forms, with no `i`, which is what makes the four 0x3F666666/0x3E4CCCCD/0xC2480000/0 argument bytes floats rather than integers.
//   This is the visible difference from the sibling setters 0x004A29C0 and 0x004A29E0, whose readers use the INTEGER forms `fiadd` and `fisub`. All five
//   slots are declared as dwords below, which is the width every access proves; the four float-kind slots hold their bits unchanged.
// Behavior/CFG: one straight-line body, no frame of its own, no saved register, no branch, no loop, no call, no local state. It stores its five dword
//   arguments into 0x00935C04, 0x0093603C, 0x00936040, 0x00936044 and 0x00936048, in that order, and returns.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Five arguments, read before any frame exists, at [esp+4], [esp+8], [esp+0xc], [esp+0x10] and [esp+0x14].
//   Declared void because the body never writes eax after the loads: retail leaves the fifth argument in ecx and the fourth in eax at the ret, so a
//   caller-visible value there is the caller's own register, not a result this routine computes, and the only caller discards it. The five slots hold
//   image value 0 (they are above the file-backed end of .data at 0x00550000) and this routine is their only writer. The meaning of the slots is not
//   established by this body; all five readers sit inside untranscribed functions, so nothing here proves what the values configure. Domain: render/,
//   the camera module that 0x004A2A00 and the sibling setters 0x004A29C0 and 0x004A29E0 belong to.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00935C04: holds the literal address the only caller passes; 0x0093603C, 0x00936040, 0x00936044 and 0x00936048: the four slots whose readers use
// the x87 floating-point forms `fmul` and `fadd`. Nothing else in the tree owns any of the five yet.
std::uint32_t g_935c04;
std::uint32_t g_93603c;
std::uint32_t g_936040;
std::uint32_t g_936044;
std::uint32_t g_936048;

void NATIVE_DECL Render_StoreArgs935C04_4A28B0(std::uint32_t arg1, std::uint32_t arg2, std::uint32_t arg3,
                                               std::uint32_t arg4, std::uint32_t arg5)
{
    ::g_935c04 = arg1;                               // 0x004A28BC: mov [0x00935C04],eax
    ::g_93603c = arg2;                               // 0x004A28C5: mov [0x0093603C],ecx
    ::g_936040 = arg3;                               // 0x004A28CF: mov [0x00936040],edx
    ::g_936044 = arg4;                               // 0x004A28D5: mov [0x00936044],eax
    ::g_936048 = arg5;                               // 0x004A28DA: mov [0x00936048],ecx; 0x004A28E0: ret
}

} // extern "C"

#undef NATIVE_DECL
