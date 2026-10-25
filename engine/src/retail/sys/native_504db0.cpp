// Native: Sys_GetGlobal16_504DB0; retail VA 0x00504DB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00504DB0-0x00504DB5 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf over the exact image (0x00504DB0-0x00504DB5; the ret is at 0x00504DB6 and two nops follow) and the raw
//   bytes 66 a1 84 80 ba 00 c3; `px 2 @ 0x00BA8084` reads the image value 0; an instruction-level sweep of build/retail-dis.txt gives seven
//   references to 0x00BA8084 across the image, of which **exactly one writes it**: 0x00504C77 `mov %ax,0x00BA8084`. The other five are reads at
//   0x00504C82, 0x00504C95, 0x00504CB6, 0x00504CD2 and 0x00504D90, all inside the function at 0x00504Cxx, plus this getter's own read at
//   0x00504DB0. All seven references use a 16-bit access, which is what fixes the object's width.
// Behavior/CFG: loads 2 byte(s) from the static object at 0x00BA8084 into %ax and returns; no arguments, no branch, no store, no callee. Only
//   %ax is written, so the declared return type is exactly the access width.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **Storage class `B`, not `D`**: the image holds 0 at this address, so it is zero-initialised
//   data and the declaration carries no initialiser - the same rule as 0x005BDDA0 in this lane. The value's meaning is not established by this
//   body; the access width and the image value are the proven facts about the object. **This file is the object's defining file in the tree.** The
//   image's only writer is 0x00504C77 inside the function at 0x00504Cxx, which is not transcribed yet (native gap above 0x0048FFFF); when it is
//   transcribed, it must DECLARE this object `extern` rather than define it again, because the routine that writes a global is its owner and the
//   getter that reads it only holds the storage until that writer exists. This object does NOT hold 0x0181, unlike the 0x0181 cluster members in
//   this lane: its image value is 0 and the only writer stores a value read elsewhere in its own function, so it is recorded as a plain 16-bit
//   state object whose semantics are unestablished. Scope note: this address is above 0x0048FFFF and outside this file's author's
//   0x0046xxxx-0x0048xxxx band; it was written under an explicit one-off scope extension from the team lead, because core/native_465a10.cpp is
//   blocked on exactly this symbol.
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

// 0x00BA8084: the 2-byte object this getter reads. Image value 0, so it is zero-initialised data. No other owner in the tree; the extent is
// proven by the 16-bit access width used by every reference to this address in the image.
std::uint16_t g_ba8084;

std::uint16_t NATIVE_DECL Sys_GetGlobal16_504DB0(void)
{
    NATIVE_LOG_ENTER("Sys_GetGlobal16_504DB0", "0x00504DB0");
    return g_ba8084;                                                     // 0x00504DB0: mov 0x00BA8084,%ax; 0x00504DB5: ret
}

} // extern "C"

#undef NATIVE_DECL
