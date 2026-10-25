// Native: Display_GetActiveMode_491CF0 (placeholder until VERIFIED); retail VA 0x00491CF0-0x00491CF5 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: build/retail-dis.txt (GNU objdump) bytes b8 40 8b c2 00 c3; radare2 6.2.0 af/afij (6 B, 1 block, 2 instructions, 0 edges); the two call sites already described in this tree push two arguments (0x00402FF0 pushes 0x00404390 and 8; terrain/view code records the same shape at engine/src/retail/terrain/terrain_draw.cpp:56 and engine/src/retail/render/view_block.cpp:11).
// Behavior/CFG: one basic block, no branch and no callee: mov $0x00C28B40,%eax; ret. It returns the address constant of the retail display object; both incoming arguments are inert because the body never reads them, and the callers keep pushing them.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 2-instruction listing; runtime UNOBSERVED. The return is retail's immediate address, not a port object: the tree's mirror of 0x00C28B40 (HostDisplayMode g_display_mode) sits in view_block.cpp's anonymous namespace and is deliberately internal to that file, and the only transcribed caller of this native (0x00402FF0) discards the value, so nothing in the port dereferences it.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void* NATIVE_DECL Display_GetActiveMode_491CF0(void* const argument1, std::int32_t const argument2)
{
    (void)argument1;                                    // pushed by the callers, never read by the body
    (void)argument2;                                    // pushed by the callers, never read by the body
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x00C28B40U)); // 0x00491CF0: mov $0xc28b40,%eax
}                                                                             // 0x00491CF5: ret

} // extern "C"

#undef NATIVE_DECL
