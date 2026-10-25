// Native: Render_StoreArgs936020_4A29C0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004A29C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004A29C0-0x004A29DD (30 bytes, 1 block, 0 edges, 7 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf over the exact image, plus the raw bytes read at the VA (8b442404 8b4c2408 8b54240c a320609300 890d28609300 89152c609300 c3):
//   three dword loads of the three arguments at 0x004A29C0/0x004A29C4/0x004A29C8, then `mov [0x00936020],eax` at 0x004A29CC, `mov [0x00936028],ecx` at
//   0x004A29D1, `mov [0x0093602C],edx` at 0x004A29D7 and `ret` at 0x004A29DD. 0x004A29DE-0x004A29DF are two nops and 0x004A29E0 is the next function
//   in this family, so the extent above is the whole routine. The only caller in the image is 0x00415530 Game_Init, which calls it at 0x004157A7 after
//   pushing 0x00539564 ("LOCAL:common/sprites/battery.spr"), 4 and 2, so the three slots receive the address of that literal, 4 and 2 in that order.
//   Each of the three slots has exactly one reader besides this store, and their instruction forms fix the width: 0x00936020 is read at 0x004A2977 with
//   `mov ecx,dword ptr [0x936020]` (a dword, consistent with the literal address stored here), while 0x00936028 is read at 0x004A4ED3 with `fiadd dword
//   ptr [0x936028]` and 0x0093602C at 0x004A4EEF with `fiadd dword ptr [0x93602c]` - the `fi` prefix is the x87 INTEGER add, so those two slots hold
//   32-bit integers and not floats. All three are declared as dwords below, which is the width every access proves. The two slots written by
//   0x004A29E0 (0x00936030 and 0x00936034) are the same kind: their readers are `fisub`, in the same function that `fiadd`s these two.
// Behavior/CFG: one straight-line body, no frame of its own, no saved register, no branch, no loop, no call, no local state. It stores its three
//   dword arguments into 0x00936020, 0x00936028 and 0x0093602C, in that order, and returns.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Three arguments, read before any frame exists, at [esp+4], [esp+8] and [esp+0xc]. Declared void
//   because the body never writes eax after the loads: retail leaves the last loaded argument in eax at the ret, so a caller-visible value there is the
//   caller's own register, not a result this routine computes, and the only caller discards it. The three slots hold image value 0 (they are above the
//   file-backed end of .data at 0x00550000) and this routine is their only writer. The meaning of the slots is not established by this body; the two
//   `fiadd` readers sit inside untranscribed functions. Domain: render/, the camera module that 0x004A2A00 and the sibling setters 0x004A29E0 and
//   0x004A28B0 belong to.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00936020: holds the literal address the only caller passes; 0x00936028 and 0x0093602C: the two integer slots its `fiadd` readers use.
// Nothing else in the tree owns any of the three yet.
std::uint32_t g_936020;
std::uint32_t g_936028;
std::uint32_t g_93602c;

void NATIVE_DECL Render_StoreArgs936020_4A29C0(std::uint32_t arg1, std::uint32_t arg2, std::uint32_t arg3)
{
    ::g_936020 = arg1;                               // 0x004A29CC: mov [0x00936020],eax
    ::g_936028 = arg2;                               // 0x004A29D1: mov [0x00936028],ecx
    ::g_93602c = arg3;                               // 0x004A29D7: mov [0x0093602C],edx; 0x004A29DD: ret
}

} // extern "C"

#undef NATIVE_DECL
