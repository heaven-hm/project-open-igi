// Native: Task_HumanPlayerHandler_412420 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00412420.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00412420-0x00412431 (18 bytes, 1 block, 0 edges, 7 instructions: 8b442408 / 6a00 / 8b08 / 51 / e832bc0400 / 83c408 / c3; the next byte 0x00412432 is nop padding).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the raw bytes decode as `mov eax,[esp+8]` (the second argument), `push 0`, `mov ecx,[eax]`, `push ecx`, `call 0x0045E060`, `add esp,8`, `ret`. 0x0040FE30 links this address as a HumanPlayer handler through 0x00401400 with the event id from Script_PhaseIdGetE4_4F1A60 (0x004F1A60). The callee 0x0045E060 is untranscribed: docs/fidelity/evidence/chronological-native-order.md row 2327 records it MISSING with size 0x35a and 7 blocks, which matches my own measurement of 858 bytes / 7 blocks / 237 instructions, and r2 afvd recovers exactly one argument (arg_18h) for it, so the caller's `push 0` is an inert argument and `push ecx` is the single real one (cdecl: the last push is the first argument).
// Behavior/CFG: loads the 32-bit value at the address held in the second argument and passes it to 0x0045E060; the first argument is never read, there is no branch, no loop and no other memory access.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The second argument slot is real, not stale: both phase-0 dispatchers (0x004012CD and 0x004013DD) push two arguments before `call *eax`, so a handler linked through 0x00401400 receives two. The callee's symbol here is an address-suffixed placeholder because no name is established and the candidate records disagree (AI_CheckSector_16 in docs/RE_AGENT_VERIFIED_NATIVES.md, Input_SamplePlayerFrame in docs/DISCOVERED_NATIVES_RECORD.md); the declared std::int32_t parameter and return follow the candidate signature int32_t(int32_t) plus my own argument-count measurement, so they are the best-supported shape rather than a recovered type, and this body does not use the result. Domain: core/, because it is a handler of the 0x00402930 startup registrar 0x0040FE30. Codegen note: GCC emits the call as a tail call - `mov eax,[esp+8]; mov eax,[eax]; mov [esp+4],eax; jmp 0x0045E060` - which is retail's call with the inert push elided and the argument slot reused; the call, its single argument and the return pass-through are unchanged.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0045E060: untranscribed callee. Only its argument count (one, r2 afvd arg_18h) and its 4-byte slot are established;
// the int32 parameter and return follow the candidate signature and are not a recovered type.
std::int32_t Core_Unknown_45E060(std::int32_t argument);

void NATIVE_DECL Task_HumanPlayerHandler_412420(void*, std::int32_t* value) // 0x00412420: the first argument is never read
{
    (void)Core_Unknown_45E060(*value); // 0x00412426/0x00412428: mov ecx,[eax]; push ecx (0x00412424's push 0 is inert)
}                                      // 0x00412431: ret

} // extern "C"

#undef NATIVE_DECL
