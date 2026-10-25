// Native: DynObj_UpdateDynTree_11 (this tree's existing name for retail VA 0x004CE3D0, recorded in terrain/terrain_loaders.cpp and include/igi/retail/terrain/terrain_loaders.hpp; the measured body is a single field clear, so the name is a placeholder to revisit, not a verified description). retail VA 0x004CE3D0-0x004CE3DE (15 bytes, 1 block, 0 edges, 3 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret) over the exact image; the extent is bounded by the `ret` at 0x004CE3DE, and the one alignment nop `90` at 0x004CE3DF ends it (next entry 0x004CE3E0). Exact bytes 8b 44 24 04 c7 80 c8 0d 00 00 00 00 00 00 c3 = `mov %eax,[esp+4]` / `movl $0x0,0xDC8(%eax)` / `ret`. No call instruction in the body; call census from the image's 0xE8 enumeration shows 0x004C6240 (Terrain_Load) as the only caller, and this tree already records the same VA as the value Terrain_Load's single exit 0x004C666C returns.
// Behavior/CFG: one cdecl pointer argument (the context at [esp+4]); stores the dword 0 at context+0xDC8 and returns nothing - retail leaves EAX holding the argument word and no caller consumes it, so the port declares void. One block, no branch, no callee.
// Status/limits: TRANSCRIBED; LINKABLE (no callee and no global, so it adds no new dependency); STRUCTURALLY_VERIFIED against the 3-instruction listing (1 block, 0 edges); runtime UNOBSERVED. The field at +0xDC8 is not interpreted: retail states no meaning for it in this function and the surrounding context layout is only partially established, so the offset is written literally rather than through a named field.

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

void NATIVE_DECL DynObj_UpdateDynTree_11(void* context)
{
    NATIVE_LOG_ENTER("DynObj_UpdateDynTree_11", "0x004CE3D0");
    // 0x004CE3D0: `mov eax,[esp+4]` - the context argument; 0x004CE3D4: `mov dword [eax+0xDC8],0` - the only write.
    *reinterpret_cast<std::uint32_t*>(static_cast<std::uint8_t*>(context) + 0xDC8) = 0;
    // 0x004CE3DE: `ret`. The argument word is left in EAX by retail; nothing reads it.
}

} // extern "C"
