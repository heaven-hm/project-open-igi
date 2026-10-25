// Native: AIGraph_InitStep_4EDCE0 (blessed address-suffixed placeholder; retail symbol not established); retail VA 0x004EDCE0-0x004EDD1A.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 59 bytes (0x3B), 1 block, 0 edges,
//   16 instructions, counted over the exact image. Extent ends at the `ret` at 0x004EDD1A; 0x004EDD1B-0x004EDD1F is `0x90` padding and the next routine starts at 0x004EDD20.
// Tools/evidence: radare2 6.2.0 `pd 22 @ 0x4edce0`, plus a byte-level image scan for `E8`/`E9` targets and for the absolute addresses of the globals.
//   Complete extent: 0x004EDCE0 push 0; 0x004EDCE2 call 0x004017C0; 0x004EDCE7 push 0; 0x004EDCE9 mov byte [0x0054DC30],al;
//   0x004EDCEE call 0x004017C0; 0x004EDCF3 push 0; 0x004EDCF5 mov byte [0x0054DC31],al; 0x004EDCFA call 0x004017C0; 0x004EDCFF push 0xFFFFFFFF;
//   0x004EDD01 push 0; 0x004EDD03 push 0x004EDD20; 0x004EDD08 push 0x0054DC34; 0x004EDD0D mov byte [0x0054DC32],al; 0x004EDD12 call 0x004B8890;
//   0x004EDD17 add esp,0x1C; 0x004EDD1A ret. The single `add esp,0x1C` releases 7 dwords: 3 for the three 0x004017C0 calls and 4 for 0x004B8890.
//   Callees: 0x004017C0 = QTask_AllocateEvent (transcribed, core/task_types.cpp); 0x004B8890 = Script_QHashNodeRegisterEx (transcribed, script/native_4b8890.cpp).
//   The string at 0x0054DC34 was read from the image and is exactly `DefinePhysicsObjType` (verified byte-for-byte, including the terminator).
//   IMAGE SCAN RESULT: 0x004EDD20 is NEVER the target of an `E8` or an `E9` anywhere in the image - it is only ever pushed as an address here - so it is
//   passed as a cast integer and deliberately NOT declared, because declaring it would emit a relocation and that relocation is what the linker reports.
//   The three globals 0x0054DC30/0x0054DC31/0x0054DC32 are each referenced from 4 sites in the image, so they are declared here and not defined.
// Behavior/CFG: no loop, no conditional branch - one straight-line block. It allocates three event ids (argument 0 each time) and keeps only the low byte
//   of each result, storing them at 0x0054DC30, 0x0054DC31 and 0x0054DC32 in that order; then it registers the script type name `DefinePhysicsObjType` with
//   the handler address 0x004EDD20, the constants 0 and -1, through 0x004B8890. The push order at the call site is -1, 0, 0x004EDD20, 0x0054DC34, so the
//   callee receives (name, 0x004EDD20, 0, -1) in memory order. Note the interleaving retail uses: each `push 0` for the NEXT allocation is issued before the
//   `mov` that stores the PREVIOUS result, so the observable order is allocate/store, allocate/store, allocate/store, then the registration call.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the disassembly above; runtime UNOBSERVED. Recorded limits: (a) the name is the blessed
//   address-suffixed placeholder; the registered name `DefinePhysicsObjType` names the registered TYPE, not this routine, so it is not used as a function name.
//   (b) 0x004EDD20 is untranscribed and deliberately undeclared - it is address-taken only, per the handler rule; the frontier for it stays open. (c) The three
//   byte globals are declared extern and their owner is not established; this file therefore reports them undefined until their owner lands. (d) The string is
//   passed as a pointer to a byte-identical literal rather than as retail's absolute address 0x0054DC34, which is the established convention for string
//   literals whose retail address is a pointer into static data.
#include <cstdint>
#include <igi/retail/core/task_types.hpp>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// 0x0054DC30/31/32: byte-wide event ids, each written here and read or written at three other sites in the image. Declared, not defined: their owner is not established.
extern std::uint8_t g_54dc30;
extern std::uint8_t g_54dc31;
extern std::uint8_t g_54dc32;

extern "C" {

// 0x004B8890: four-argument script type registrar (script/native_4b8890.cpp).
void Script_QHashNodeRegisterEx(void* const arg0, void* const arg1, char const* const arg2, void* const arg3);

void NATIVE_DECL AIGraph_InitStep_4EDCE0()
{
    // 0x004EDCE0-0x004EDCE9: allocate with argument 0; the low byte of the result goes to 0x0054DC30.
    g_54dc30 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U));

    // 0x004EDCEE-0x004EDCF5: allocate with argument 0; the low byte of the result goes to 0x0054DC31.
    g_54dc31 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U));

    // 0x004EDCFA-0x004EDD0D: allocate with argument 0; the low byte of the result goes to 0x0054DC32.
    g_54dc32 = static_cast<std::uint8_t>(QTask_AllocateEvent(0U));

    // 0x004EDCFF-0x004EDD12: register "DefinePhysicsObjType" with handler 0x004EDD20, then 0 and -1.
    Script_QHashNodeRegisterEx(
        const_cast<char*>("DefinePhysicsObjType"),                            // 0x004EDD08: push 0x0054DC34 (the string bytes)
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0x4EDD20U)),     // 0x004EDD03: push 0x004EDD20 (address-taken only; deliberately undeclared)
        nullptr,                                                             // 0x004EDD01: push 0
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xFFFFFFFFU)));  // 0x004EDCFF: push -1
}

} // extern "C"

#undef NATIVE_DECL
