// Native: AIGraph_Handler_509440; retail VA 0x00509440 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509440-0x0050944C (13 bytes, 1 block, 0 edges, 4 instructions: 8b442408 8b0dccb2a700 8908 c3).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00509440-0x0050944C; radare2 6.2.0 afij; linked by the AIGraph registrar 0x00508FA0 at 0x00509256 (0x00401400 event 0x004F1A60()).
// Behavior/CFG: one block, no branch, no call. Stores the AIGraph parameter container word 0x00A7B2CC through the second argument; the first argument is never read (no prologue, so the store target is the word at [esp+8] = arg2).
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 4-instruction listing; runtime UNOBSERVED. The container word is defined by core/native_508fa0.cpp (the registrar that grows it at 0x00509131), so it is extern here.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A7B2CC: the parameter container grown by the AIGraph registrar 0x00508FA0 (g_a7b2cc, defined there).
extern void* g_a7b2cc;

void NATIVE_DECL AIGraph_Handler_509440(void* context, void** out)
{
    (void)context;                            // arg1 unread: retail's first instruction targets the word at [esp+8]
    *out = g_a7b2cc;                          // 0x00509444: mov 0xa7b2cc,%ecx; 0x0050944A: mov %ecx,(%eax)
}                                             // 0x0050944C: ret
} // extern "C"

#undef NATIVE_DECL
