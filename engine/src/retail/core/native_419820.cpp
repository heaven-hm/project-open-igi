// Native: BackgroundFX_UpdateRecord_419820; retail VA 0x00419820 (0x00419820-0x0041985D, 62 bytes, 20 instructions, 1 block, 0 edges).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 byte+instruction listing: 56 8b 74 24 08 8b 46 20 89 44 24 08 33 c0 8b 4c 24 08 a3 50 4a a8 00 51 56 a3 58 4a a8 00 e8 ... d9 44 24 10 d8 05 e0 33 53 00 83 c4 04 d9 1c 24 56 e8 ... 83 c4 08 5e c3); the one-argument handler ABI is the one 0x00419760 links into the class handler table at event index 1; radare2 6.2.0 af/afij (size 62, 1 block, 0 edges, 20 instructions); 0x00419760 installs this as the handler returned by 0x004E7530 for the class id at 0x00539AF0.
// Behavior/CFG: one cdecl argument read (a record pointer); the retail body also uses its second argument slot as scratch (see below). Straight-line: reads the float at record+0x20, stores it into the incoming argument slot at 0x8(%esp) (0x00419828), clears the two global dwords 0x00A84A50 and 0x00A84A58, calls 0x00419860(record, value), then loads the overwritten slot as a float (0x00419843), adds the 1.0f constant at 0x005333E0, stores it back over the argument slot (0x00419850), pushes the record again and calls 0x00419860(record, value + 1.0f). No branches, no other data.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED. This file owns the only definitions of g_a84a50 and g_a84a58 (retail writes 0x00A84A50/0x00A84A58 at the lowest address here, 0x00419832/0x00419839; the other writers 0x00419D8E/0x00419D94, 0x0041D996/0x0041D99C, 0x00421E13/0x00421E27 are later). The 1.0f constant is the .rdata float at 0x005333E0 (bytes 00 00 80 3f, already documented in engine/src/retail/terrain/terrain_mipmaps.cpp:108). The callee 0x00419860 is not transcribed yet, so this file carries one undefined code symbol by design. Retail statement order preserved: the record+0x20 load and the argument-slot store (0x00419825-0x00419828), the zero in EAX (0x0041982C), the argument-slot reload (0x0041982E), the first global clear (0x00419832), the first call's pushes (0x00419837-0x00419838), the second global clear (0x00419839), the first call (0x0041983E), the float load/add at 0x00419843-0x00419847, the argument-slot overwrite (0x0041984D-0x00419850) and the second call (0x00419853-0x00419854).
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

// 0x00A84A50 and 0x00A84A58: global dwords cleared here (0x00419832, 0x00419839). Retail writes them from four sites, the
// lowest address being this one, so this file owns the definitions; every other consumer must declare them, not redefine them.
std::uint32_t g_a84a50 = 0U;
std::uint32_t g_a84a58 = 0U;

// The class updater called twice from this body; declared because it is not transcribed yet.
void NATIVE_DECL BackgroundFX_BuildAxes_419860(void* record, float angle);

// Registered as an event handler, so the task dispatcher calls it with one argument: [esp+4] record. Retail's second
// stack word at [esp+8] is caller-owned scratch that the body overwrites at 0x00419828 before using it as the float
// argument of the two 0x00419860 calls; that store is a stack-frame detail with no caller-visible effect, so this
// transcription takes the single argument the handler ABI passes and computes the same two float arguments directly.
void NATIVE_DECL BackgroundFX_UpdateRecord_419820(void* record)
{
    NATIVE_LOG_ENTER("BackgroundFX_UpdateRecord_419820", "0x00419820");
    // 0x00419821-0x00419828: `mov 0x8(%esp),%esi; mov 0x20(%esi),%eax; mov %eax,0x8(%esp)` - the record's +0x20 float is
    // written over the argument slot at [esp+8] before any read of it; the two calls below then read their float argument
    // from that overwritten slot.
    const float value = *reinterpret_cast<float*>(reinterpret_cast<std::uint8_t*>(record) + 0x20);

    // 0x0041982C-0x00419839: `xor %eax,%eax; mov 0x8(%esp),%ecx; mov %eax,0x00A84A50; push %ecx; push %esi; mov %eax,0x00A84A58`.
    g_a84a50 = 0U;
    g_a84a58 = 0U;

    // 0x0041983E: `call 0x00419860` with the record and the unchanged value.
    BackgroundFX_BuildAxes_419860(record, value);

    // 0x00419843-0x00419850: `flds 0x10(%esp); fadds 0x005333E0; add $0x4,%esp; fstps (%esp)` - the value plus 1.0f replaces
    // the argument slot the callee had. 0x005333E0 is the float 1.0f.
    const float next = value + 1.0F;

    // 0x00419853-0x00419854: `push %esi; call 0x00419860`.
    BackgroundFX_BuildAxes_419860(record, next);
}

} // extern "C"

#undef NATIVE_DECL
