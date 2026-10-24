// Native: Task_ClearPendingWord_468BB0; retail VA 0x00468BB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00468BB0-0x00468BBA (11 bytes, 1 block, 2 instructions: c7 05 68dd5b00 00000000 / c3).
// Tools/evidence: radare2 6.2.0 pd; section 21 of the evidence sheet (installed as a phase-2 handler by 0x004688B0).
// Behavior/CFG: stores zero to the dword at 0x005BDD68 and returns; no branch, no call, no stack use, no argument read.
// Status/limits: STRUCTURALLY_VERIFIED (1 block / 0 edges, emitted instructions identical); runtime UNOBSERVED.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005BDD68: the word this handler clears. No translation unit owns it.
std::uint32_t g_5bdd68;

void NATIVE_DECL Task_ClearPendingWord_468BB0(void* /*task*/)
{
    ::g_5bdd68 = 0U;   // 0x00468BB0: mov dword [0x005BDD68], 0
}                      // 0x00468BBA: ret

} // extern "C"

#undef NATIVE_DECL
