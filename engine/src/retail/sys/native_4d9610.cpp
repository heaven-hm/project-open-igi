// Native: Sys_GetGlobal16_4D9610; retail VA 0x004D9610 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004D9610-0x004D9615 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf over the exact image (0x004D9610-0x004D9615; the ret is at 0x004D9616 and two nops follow) and the raw
//   bytes 66 a1 18 86 54 00 c3; `px 2 @ 0x00548618` reads the image value 385 = 0x0181; an instruction-level sweep of build/retail-dis.txt gives
//   eleven references to 0x00548618 across the image, of which **exactly one writes it**: 0x004D95F2 `movw $0x181,0x00548618`, i.e. the image's
//   own only writer stores 0x0181 - the same value the image already holds there. The other nine are reads (0x004D756B, 0x004D7576, 0x004D758F,
//   0x004D75A3, 0x004D75B6, 0x004D75D0, 0x004D75F2, 0x004D7600, 0x004D95E0) plus this getter's own read at 0x004D9610.
// Behavior/CFG: loads 2 byte(s) from the static object at 0x00548618 into %ax and returns; no arguments, no branch, no store, no callee. Only
//   %ax is written, so the declared return type is exactly the access width. The address is 2-byte aligned and every reference in the image uses
//   a 16-bit access, which is what fixes the object's width.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The value's meaning is not established by this body; the access width and the image value are
//   the proven facts about the object. **Storage class `D`, not `B`**: the image holds 0x0181 at this address, so it is initialised data and the
//   declaration carries that initialiser - the same rule as 0x0053F6D0 in core/native_4682f0.cpp. **This file is the object's defining file in the
//   tree.** The image's only writer is 0x004D95F2 inside the function at 0x004D95xx, which is not transcribed yet (native gap in the
//   0x004Cxxxx-0x0050xxxx range); when it is transcribed, it must DECLARE this object `extern` rather than define it again, because the routine
//   that writes a global is its owner and the getter that reads it only holds the storage until that writer exists. The object is a member of the
//   0x0181 cluster (the value both the pre-registration id slots and the event allocator argument use), recorded as an observation and not as a
//   claim about what 0x0181 means. Scope note: this address is above 0x0048FFFF and outside this file's author's 0x0046xxxx-0x0048xxxx band; it
//   was written under an explicit one-off scope extension from the team lead, because core/native_464260.cpp is blocked on exactly this symbol.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00548618: the 2-byte object this getter reads. Image value 0x0181, so it is initialised data. No other owner in the tree; the extent is
// proven by the 16-bit access width used by every reference to this address in the image.
std::uint16_t g_548618 = 0x0181U;

std::uint16_t NATIVE_DECL Sys_GetGlobal16_4D9610(void)
{
    return g_548618;                                                     // 0x004D9610: mov 0x00548618,%ax; 0x004D9615: ret
}

} // extern "C"

#undef NATIVE_DECL
