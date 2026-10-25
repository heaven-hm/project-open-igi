// Native: Task_ClearGlassShatterType_44A1C0 (behavior-derived placeholder: it clears the task type whose id is the single global this routine reads; retail symbol unknown); retail VA 0x0044A1C0-0x0044A1CD.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 14 bytes (0xE), 1 block, 0 edges,
//   5 instructions, counted with af + afbj over the exact image. The body ends at the `ret` at 0x0044A1CD; fourteen 0x90 pad bytes follow at
//   0x0044A1CE-0x0044A1DB and the next function starts at 0x0044A1DC.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the objdump listing build/retail-dis.txt. The complete listing of the extent is
//   `mov 0x57c240,%ax` (0x0044A1C0, bytes 66 a1 40 c2 57 00 - a 16-bit absolute load into AX), `push %eax` (0x0044A1C6), `call 0x00401A20`
//   (0x0044A1C7), `pop %ecx` (0x0044A1CC) and `ret` (0x0044A1CD) - five instructions, no branch, no other callee, no other data reference.
//   Callers: exactly one, `call 0x44a1c0` at 0x00402DBC; the routine is never used as a linked handler, so it is not a handler body.
//   Global ownership measured over the whole listing: 0x0057C240 has exactly ONE writer image-wide, `mov %ax,0x57c240` at 0x00449E1C
//   inside Task_DefineGlassShatter_449D30, which stores TaskType_Register's return value there. Four reads reference it in total.
// Behavior/CFG: no argument is read and no value is returned. It loads the 16-bit class id at 0x0057C240 and calls the type-clear routine
//   0x00401A20 with it. The load is 16-bit, so retail's upper EAX bits are whatever the preceding instruction left in the register; the callee
//   masks its argument with 0xFFFF at its own 0x00401A25 (`and eax,0xffff`), so only the low word is observable and the port passes the
//   std::uint16_t value. The pushed argument slot is released by `pop %ecx` rather than `add $0x4,%esp`.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 5-instruction listing and afbj (14 B / 1 block / 0 edges); runtime UNOBSERVED.
//   Recorded limits: (a) the name is a behavior-derived placeholder - the routine's retail symbol is not established, and 0x0044A1C0 is kept as
//   the address suffix. (b) Whether 0x0057C240 is exclusively the "GlassShatter" id is established only as far as the listing shows: one writer
//   (the registrar that registers "GlassShatter") and four readers, so no other class stores its id there in this image. (c) What 0x00401A20
//   does beyond releasing the type's slot is that file's declared scope (core/native_401a20.cpp). (d) The single caller 0x00402DBC is not
//   transcribed yet, so whether it leaves an argument dword pending across this call is not established here; this body reads none.
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

extern "C" void TaskType_Clear_401A20(std::uint16_t class_id); // 0x00401A20: clears the type slot for a class id (core/native_401a20.cpp)

// 0x0057C240: the "GlassShatter" class id, defined by its single writer Task_DefineGlassShatter_449D30 (core/native_449d30.cpp).
extern "C" std::uint16_t g_57c240;

void NATIVE_DECL Task_ClearGlassShatterType_44A1C0()
{
    NATIVE_LOG_ENTER("Task_ClearGlassShatterType_44A1C0", "0x0044A1C0");
    // 0x0044A1C0: mov 0x57c240,%ax; 0x0044A1C6: push %eax; 0x0044A1C7: call 0x401a20; 0x0044A1CC: pop %ecx; 0x0044A1CD: ret.
    TaskType_Clear_401A20(::g_57c240);
}

} // extern "C"

#undef NATIVE_DECL
