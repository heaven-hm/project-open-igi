// Native: List_GetObject_4F1030; retail VA 0x004F1030 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x004F1030-0x004F104F; radare2 6.2.0 pd/afij (32 bytes, 5 blocks, 12 instructions); the table base 0x00A71890 and the 0x1000 bound are the immediates at 0x004F103F/0x004F1038, matching the 0x1000-dword clear in 0x004EE982; called from 0x004F1244 and 0x004F1252.
// Behavior/CFG: five blocks, no loop. Rejects an index below zero or at/above 0x1000 with null, reads the table entry 0x00A71890[index], rejects a null entry, and returns the dword the entry points at (the object pointer stored at slot+0).
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the 12-instruction listing; runtime UNOBSERVED. Leaf: no call; the only global is
//                the 0x00A71890 table already defined in script/script_state_globals_4ee970.cpp.
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

extern std::uint32_t g_a71890[0x1000];   // 0x00A71890, defined in engine/src/retail/script/script_state_globals_4ee970.cpp

void* NATIVE_DECL List_GetObject_4F1030(std::int32_t index)
{
    NATIVE_LOG_ENTER("List_GetObject_4F1030", "0x004F1030");
    if (index < 0)                                                                    // 0x004F1034/0x004F1036: test %eax,%eax; jl 0x4f104d
    {
        return nullptr;                                                               // 0x004F104D: xor %eax,%eax
    }
    if (index >= 0x1000)                                                              // 0x004F1038/0x004F103D: cmp $0x1000,%eax; jge 0x4f104d
    {
        return nullptr;
    }
    void* const entry = reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_a71890[index])); // 0x004F103F: mov 0xa71890(,%eax,4),%eax
    if (entry == nullptr)                                                             // 0x004F1046/0x004F1048: test %eax,%eax; je 0x4f104d
    {
        return nullptr;
    }
    return *static_cast<void**>(entry);                                               // 0x004F104A: mov (%eax),%eax
}                                                                                     // 0x004F104C: ret
} // extern "C"

#undef NATIVE_DECL
