// Native: Script_QHashNodeRemove_4C0460 (behavior-derived placeholder name; retail symbol unknown). retail VA 0x004C0460-0x004C04A4 (69 bytes, 3 blocks, 3 edges, 25 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x004C0460 plus capstone 5.0.7 aligned from the verified entry; both agree on 25 instructions. Exact bytes 8b 4c 24 04 56 8b 74 24 0c 8d 44 24 0c 56 50 51 e8 3b 00 00 00 83 c4 0c 84 c0 74 19 8b 54 24 0c 52 e8 da f4 fe ff 8b 44 24 10 50 e8 80 08 ff ff 83 c4 08 5e c3 56 68 48 7e 54 00 e8 10 f3 fe ff 83 c4 08 eb fe = `mov ecx,[esp+4]` / `push %esi` / `mov esi,[esp+0xc]` / `lea eax,[esp+0xc]` / `push %esi` / `push %eax` / `push %ecx` / `call 0x004C04B0` / `add $0xc,%esp` / `test al,al` / `je 0x004C0495` / `mov edx,[esp+0xc]` / `push %edx` / `call 0x004AF960` / `mov eax,[esp+0x10]` / `push %eax` / `call 0x004B0D10` / `add $0x8,%esp` / `pop %esi` / `ret` / `push %esi` / `push $0x547E48` / `call 0x004AF7B0` / `add $0x8,%esp` / `jmp 0x004C04A3`. Every call displacement resolves as v+5+rel32: 0x004C0475 + 0x0000003B = 0x004C04B0; 0x004C0486 + 0xFFFEF4DA = 0x004AF960; 0x004C0490 + 0xFFFF0880 = 0x004B0D10; 0x004C04A0 + 0xFFFEF310 = 0x004AF7B0. The self-jump `eb fe` is at 0x004C04A3-0x004C04A4, so the extent is 69 bytes, and 0x004C04A5 onward is `nop` alignment padding. Call census from the whole image's 0xE8 enumeration: 0x004BAB4F in Script_QHashSymbolRemove_4BAB30 and 0x004B8945 (in the 0x004B8920 cluster) reach it; the fatal path's string at 0x00547E48 is "Unable to remove symbol \"%s\" (not registered)".
// Behavior/CFG: two arguments (list, value), cdecl, no frame. Three blocks: the entry block calls the node lookup at 0x004C04B0(list, &value, value) - the out slot being the ADDRESS OF THIS BODY'S OWN SECOND ARGUMENT WORD (0x004C0469 `lea eax,[esp+0xc]`), so the found node is written over the value word - then branches on the LOW BYTE of the result (0x004C0478 `test al,al`, 0x004C047A `je 0x004C0495`); the success block unlinks the node from its list with 0x004AF960 and frees it with 0x004B0D10 (Mem_DeAllocFn), and the fatal block reports and spins. RETAIL TESTS ONLY `al`, NOT THE WHOLE RETURN WORD: the local check below therefore casts to std::uint8_t rather than comparing the std::int32_t result with zero, because the two differ for any return whose low byte is zero while higher bytes are not. BOTH 0x004C047C AND 0x004C0486 READ THE SAME ARGUMENT WORD (the offsets 0x004C047C `[esp+0xc]` and 0x004C0486 `[esp+0x10]` differ only by the push at 0x004C0480), so the unlink and the free receive the identical node pointer the lookup wrote back, and the local `node` models that one slot. 0x004C04A3 `jmp 0x004C04A3` is retail's terminal spin and is preserved as an empty `for (;;)`. The two failure strings in this chain ("Unable to remove symbol ... not registered" here, "Undefined symbol: '%s'" in 0x004BAB30) are what establish that the chain removes symbols rather than looking them up.
// Status/limits: TRANSCRIBED; LINKABLE (all four of its callees 0x004C04B0, 0x004AF960, 0x004B0D10 and 0x004AF7B0 are tree natives); STRUCTURALLY_VERIFIED against the 25-instruction listing (3 blocks, 3 edges); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. THE NAME IS A PLACEHOLDER derived from measured callee behavior and the fatal string; it must be revisited when the script symbol table is understood as a whole. Domain: script/, because its arguments are a symbol-table pointer and a script symbol name.

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

// 0x004C04B0: looks a value up in a hash list and writes the node into the out slot (engine/src/retail/script/native_4c04b0.cpp).
std::int32_t Script_QHashNodeLookup(void* const list, void** const out, char const* const value);

// 0x004AF960: unlinks a node from its list (engine/src/retail/sys/native_4af960.cpp).
void* NATIVE_DECL Sys_UnlinkListNode_4AF960(void* node);

// 0x004B0D10: deallocation entry (engine/src/retail/mem/native_4b0d10.cpp).
void Mem_DeAllocFn(void* const pointer);

// 0x004AF7B0: the fatal-error report/dispatch (engine/src/retail/core/retail_error.cpp), cdecl varargs.
int Debug_Error(char const* format, ...);

void NATIVE_DECL Script_QHashNodeRemove_4C0460(void* const list, char const* const value)
{
    NATIVE_LOG_ENTER("Script_QHashNodeRemove_4C0460", "");
    // 0x004C0469: `lea eax,[esp+0xc]` is the address of this body's own second argument word, so the lookup writes the
    // found node over the value; `node` models that same single slot.
    void* node = nullptr;
    // 0x004C0470: `call 0x004C04B0`; 0x004C0478: `test al,al`; 0x004C047A: `je 0x004C0495` - taken when the lookup fails.
    // The low-byte test is retail's, not a whole-word comparison.
    if (static_cast<std::uint8_t>(Script_QHashNodeLookup(list, &node, value)) == 0U)
    {
        // 0x004C0495: `push esi` (the value, and the %s vararg) / 0x004C0496: `push 0x00547E48` ("Unable to remove
        // symbol \"%s\" (not registered)") / 0x004C049B: `call 0x004AF7B0`. The last push is argument 1, so the format
        // is first and the value is the vararg. 0x004C04A3: `jmp 0x004C04A3` - retail's terminal spin, preserved.
        Debug_Error("Unable to remove symbol \"%s\" (not registered)", value);
        for (;;) {}
    }
    // 0x004C047C: `mov edx,[esp+0xc]` - the second argument word, now holding the lookup's node
    Sys_UnlinkListNode_4AF960(node);
    // 0x004C0486: `mov eax,[esp+0x10]` - the same slot again / 0x004C048B: `call 0x004B0D10`
    Mem_DeAllocFn(node);
    // 0x004C0490: `add esp,0x8` is the cdecl cleanup for the two argument dwords this body pushed, emitted by the
    // compiler rather than written by hand. 0x004C0493: `pop esi` / 0x004C0494: `ret`.
}

} // extern "C"
