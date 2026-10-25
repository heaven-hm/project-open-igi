// Native: List_FirstFree_4F1240; retail VA 0x004F1240 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F1240-0x004F1261; radare2 6.2.0 pd/afij (34 bytes, 3 blocks, 16 instructions); called only from 0x005093D4 (0x005093C0 handler).
// Behavior/CFG: three blocks, one loop. Probes 0x004F1030 from index 0 upward and returns the first index whose entry resolves to a non-null object pointer (the loop stops as soon as the lookup returns null).
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 16-instruction listing; runtime UNOBSERVED. NO ARGUMENTS: the `push %esi` at
//                0x005093D3 belongs to the following 0x004F1090 call (it is that call's second argument, pushed early and cleared by the shared
//                `add $0x8,%esp` at 0x005093DF); this body pushes its own index 0 at 0x004F1243 (`xor %esi,%esi; push %esi`). Link-closed: its
//                only callee 0x004F1030 is transcribed in native_4f1030.cpp.
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

// 0x004F1030: bounds-checked 0x00A71890 lookup returning the entry's object pointer, transcribed in native_4f1030.cpp.
void* NATIVE_DECL List_GetObject_4F1030(std::int32_t index);

std::int32_t NATIVE_DECL List_FirstFree_4F1240(void)
{
    NATIVE_LOG_ENTER("List_FirstFree_4F1240", "0x004F1240");
    std::int32_t index = 0;                                                           // 0x004F1241: xor %esi,%esi
    while (List_GetObject_4F1030(index) != nullptr)                                   // 0x004F1243/0x004F1244 and 0x004F124C/0x004F125C
    {
        ++index;                                                                      // 0x004F1250: inc %esi
    }
    return index;                                                                     // 0x004F125E: mov %esi,%eax
}                                                                                     // 0x004F1261: ret
} // extern "C"

#undef NATIVE_DECL
