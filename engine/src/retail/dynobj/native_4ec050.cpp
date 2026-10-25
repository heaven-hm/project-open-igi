// Native: Sub_4EC050 (blessed address-suffixed placeholder; retail symbol not established); retail VA 0x004EC050-0x004EC055.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 6 bytes (0x6), 1 block, 0 edges, 2 instructions.
// Tools/evidence: ndisasm 3.01 (`-b32`) over the exact 6 bytes obtained through scripts/native_fidelity_audit.py RetailImage.va_to_off(); the bytes are
//   `a0 81 db 54 00 c3` = `mov al,byte [0x0054DB81]` (5 bytes) then `ret` (1 byte), and the byte accounting closes (2 instructions over all 6 bytes). The
//   `ret` is at 0x004EC055 and the byte after it is `0x90` padding, so 0x004EC056 is not part of this routine. The global's value is read from the image.
// Behavior/CFG: returns the byte at 0x0054DB81; one straight-line block, no branch, no call, no store, no argument. It never reads the stack, so any push
//   before a call to it is ignored by this callee and is available to the next callee as a deferred argument - which is exactly how
//   core/native_51b400.cpp uses it (the push at 0x0051B45A is the handler of the 0x00401400 call at 0x0051B46D, not this routine's argument).
//   SUBSYSTEM, inferred rather than established: 0x0054DB81 is referenced from exactly five sites in the image (0x004EBE86, 0x004EBEE9, 0x004EC002,
//   0x004EC01B, 0x004EC051) and every one of them lies inside the 0x004EBE00-0x004EC140 band, which the external name table calls the AnimTrigger family
//   (AnimTrigger_Shutdown 0x004EBFC0, AnimTrigger_ParseConfigFile 0x004EC070, AnimTriggerLoad 0x004EC0C0, AnimTrigger_Fire 0x004EC0E0,
//   AnimTriggerEvent_Init 0x004EC160, StateMachine_Init 0x004EC190). The table names this address AnimTrigger_GetSizeEvent. That table is EXTERNAL AND
//   UNTRUSTED and the 6-byte body cannot confirm a semantic identity, so the address-suffixed name is kept and the file is filed under `dynobj`, the tree's
//   domain for dynamic interactive objects. This placement is an inference from the reference-site evidence, not a verified subsystem claim.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the disassembly above; runtime UNOBSERVED. Recorded limits: (a) the name is the blessed
//   address-suffixed placeholder; `AnimTrigger_GetSizeEvent` is an unverified external candidate. (b) Codegen note: retail writes only AL, leaving EAX's
//   upper bytes as they were; a std::uint8_t return zero-extends EAX. That is the same documented deviation as sys/native_4c56f0.cpp and 0x00413B20.
//   (c) The global 0x0054DB81 is defined here because no other file in the tree owns it; its definition carries 0xFF, the value the image holds at that
//   address (file-backed .data, verified byte 0xFF). Callers that depend on the upper 24 bits of EAX must restore them, and core/native_51b400.cpp does.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0054DB81: the byte this getter returns. File-backed .data holding 0xFF in the image; written by the AnimTrigger band and owned by no other tree file.
std::uint8_t g_54db81 = 0xFFU;

std::uint8_t NATIVE_DECL Sub_4EC050(void)
{
    return ::g_54db81; // 0x004EC050: mov al,byte [0x0054DB81]
} // 0x004EC055: ret

} // extern "C"

#undef NATIVE_DECL
