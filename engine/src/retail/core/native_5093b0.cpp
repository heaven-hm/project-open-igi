// Native: AIGraph_Handler_5093B0; retail VA 0x005093B0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x005093B0-0x005093BB (12 bytes, 1 block, 0 edges, 5 instructions: 8b442404 50 e826ffffff 59 c3).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x005093B0-0x005093BB; radare2 6.2.0 afij; linked by the AIGraph registrar 0x00508FA0 at 0x00509000 (0x00401400 event 0x004F9280()).
// Behavior/CFG: one block, no branch. Pushes the incoming word and tail-calls the resource release 0x005092E0 on it, then returns.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 5-instruction listing; runtime UNOBSERVED. Link-blocked on 0x005092E0
//                (not transcribed yet); its prototype here comes from this call site.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005092E0: the AIGraph state release (88 bytes, 5 blocks, 27 instructions; calls 0x004B0D60 and 0x004B0D10). NOT transcribed yet.
void NATIVE_DECL AIGraph_ResetState_5092E0(void* object);

void NATIVE_DECL AIGraph_Handler_5093B0(void* object)
{
    AIGraph_ResetState_5092E0(object);   // 0x005093B4: mov 0x4(%esp),%eax; 0x005093B5: push %eax; call 0x5092E0; 0x005093BA: pop %ecx
}                                        // 0x005093BB: ret
} // extern "C"

#undef NATIVE_DECL
