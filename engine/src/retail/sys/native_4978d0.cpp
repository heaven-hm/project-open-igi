// Native: Sys_AddSlotName_4978D0; retail VA 0x004978D0-0x004978DD (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004978D0-0x004978DD
//   (14 bytes, 1 block, 5 instructions; no branches).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; the body is `mov eax,[esp+4]; push eax; call 0x004B0E70; add esp,4; ret`, and
//   0x004B0E70 is the transcribed sys/native_4b0e70.cpp (Sys_AddSlot_4B0E70, const char* name), so this is a one-argument forwarder with the
//   callee's own ABI rather than a new algorithm.
// Behavior/CFG: passes its single stack argument through to 0x004B0E70 and returns; the only stack traffic is the callee's argument push and its
//   matching `add esp,4`. No branching, no state of its own.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The body performs no operation on EAX, so the value in EAX on return is whatever the callee
//   leaves there; 0x004B0E70 is declared void, so this forwarder is declared void too rather than inventing a return type for it. The callee's
//   error path (Debug_Error plus spin) is the callee's, not reproduced here.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" void Sys_AddSlot_4B0E70(const char* name); // 0x004B0E70

extern "C" {

void NATIVE_DECL Sys_AddSlotName_4978D0(const char* name)
{
    Sys_AddSlot_4B0E70(name); // 0x004978D0: mov eax,[esp+4]; 0x004978D4: push eax; 0x004978D5: call 0x004B0E70; 0x004978DA: add esp,4
    // 0x004978DD: ret (EAX is the callee's leftover, not written here).
}

} // extern "C"

#undef NATIVE_DECL
