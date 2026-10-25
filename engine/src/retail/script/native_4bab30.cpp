// Native: Script_QHashSymbolRemove_4BAB30 (behavior-derived placeholder name; retail symbol unknown; it is the worker behind the twelve-byte forwarder 0x004B8920, which is declared against this same name). retail VA 0x004BAB30-0x004BAB7C (76 bytes, 3 blocks, 3 edges, 26 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x004BAB30 plus capstone 5.0.7 aligned from the verified entry; both agree on 26 instructions. Exact bytes 56 8b 74 24 08 8d 44 24 08 50 56 e8 40 00 00 00 83 c4 08 85 c0 56 74 25 8b 0d 88 3e a4 00 51 e8 0c 59 00 00 8b 54 24 10 52 e8 32 ff ff ff 8b 44 24 14 50 e8 a8 61 ff ff 83 c4 10 5e c3 68 fc 76 54 00 e8 39 4c ff ff 83 c4 08 eb fe = `push %esi` / `mov esi,[esp+8]` / `lea eax,[esp+8]` / `push %eax` / `push %esi` / `call 0x004BAB80` / `add $0x8,%esp` / `test eax,eax` / `push %esi` / `je 0x004BAB6D` / `mov ecx,[0x00A43E88]` / `push %ecx` / `call 0x004C0460` / `mov edx,[esp+0x10]` / `push %edx` / `call 0x004BAA90` / `mov eax,[esp+0x14]` / `push %eax` / `call 0x004B0D10` / `add $0x10,%esp` / `pop %esi` / `ret` / `push $0x5476FC` / `call 0x004AF7B0` / `add $0x8,%esp` / `jmp 0x004BAB7A`. Every call displacement resolves as v+5+rel32: 0x004BAB40 + 0x00000040 = 0x004BAB80; 0x004BAB54 + 0x0000590C = 0x004C0460; 0x004BAB5E + 0xFFFFFF32 = 0x004BAA90; 0x004BAB68 + 0xFFFF61A8 = 0x004B0D10; 0x004BAB77 + 0xFFFF4C39 = 0x004AF7B0. The self-jump `eb fe` is at 0x004BAB7A-0x004BAB7B, so the extent is 76 bytes, and 0x004BAB7C onward is alignment padding. Call census from the whole image's 0xE8 enumeration: 0x004B8920 calls this body and nothing else does, so it is reached only through the forwarder; the fatal path's string at 0x005476FC is "Undefined symbol: '%s'".
// Behavior/CFG: one argument (name), cdecl, no frame. 0x004BAB35 `lea eax,[esp+8]` takes the address of THIS BODY'S OWN ARGUMENT WORD, so the lookup at 0x004BAB80(name, &argument_word) overwrites the name with its out value; the local `node` below models that one slot. Three blocks: the entry block tests the lookup result (0x004BAB43 `test eax,eax`, 0x004BAB46 `je 0x004BAB6D` - the branch is taken when the lookup FAILS), the success block removes the symbol, and the fatal block reports and spins. THE `push %esi` AT 0x004BAB45 IS THE FATAL PATH'S VARARG, NOT DEAD CODE: it is pushed before the branch, so on the fatal path the stack is [format at 0x004BAB6D's push][name from 0x004BAB45] and 0x004AF7B0 receives (format, name) - which is what binds the `%s`; on the success path the same pushed word is the second argument of 0x004C0460. That is why the fatal path's `add $0x8,%esp` at 0x004BAB77 cleans exactly two words. The success path calls 0x004C0460(g_a43e88, name) with the symbol-table pointer loaded from 0x00A43E88, then 0x004BAA90(node) and 0x004B0D10(node) - and `node` is the value the lookup wrote into the argument slot (0x004BAB54 and 0x004BAB5E both read the slot at [esp+0x10]/[esp+0x14] after the push offsets, which is the entry-time argument word). 0x004BAB7A `jmp 0x004BAB7A` is retail's terminal spin and is preserved as an empty `for (;;)`.
// Status/limits: TRANSCRIBED; LINKABLE (its five callees 0x004BAB80, 0x004C0460, 0x004BAA90, 0x004B0D10 and 0x004AF7B0 and its global g_a43e88 are all tree natives, so it adds no new dependency); STRUCTURALLY_VERIFIED against the 26-instruction listing (3 blocks, 3 edges); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. THE NAME IS A PLACEHOLDER: it is derived from the measured callee behavior (a symbol-table removal whose two failure strings are "Undefined symbol: '%s'" here and "Unable to remove symbol \"%s\" (not registered)" in 0x004C0460) and it must be revisited when the symbol table is understood as a whole. The call to 0x004AF7B0 is a real symbol reference to the already-transcribed varargs Debug_Error (core/retail_error.cpp), not a cast. Domain: script/, because the chain is the script symbol table at 0x00A43E88 and the argument is a script symbol name.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00A43E88 - the script symbol table, created by 0x004BACD0 (see native_4bacd0.cpp).
extern "C" void* g_a43e88;

// 0x004BAB80: looks a symbol up by name and writes the node into the out slot (engine/src/retail/script/native_4bab80.cpp).
std::int32_t NATIVE_DECL Script_QHashSymbolLookup_4BAB80(const char* name, void** out_node);

// 0x004C0460: removes the named symbol from a table and frees its node (engine/src/retail/script/native_4c0460.cpp).
void NATIVE_DECL Script_QHashNodeRemove_4C0460(void* list, char const* value);

// 0x004BAA90: clears a value reference from the live node list (engine/src/retail/script/native_4baa90.cpp).
void NATIVE_DECL Script_QHashNodeClearRef_4BAA90(void* value);

// 0x004B0D10: deallocation entry (engine/src/retail/mem/native_4b0d10.cpp).
void Mem_DeAllocFn(void* const pointer);

// 0x004AF7B0: the fatal-error report/dispatch (engine/src/retail/core/retail_error.cpp), cdecl varargs.
int Debug_Error(char const* format, ...);

void NATIVE_DECL Script_QHashSymbolRemove_4BAB30(char const* name)
{
    // 0x004BAB31: `mov esi,[esp+8]` - the name. 0x004BAB35: `lea eax,[esp+8]` is the address of retail's own argument
    // word, so the lookup writes the found node over the name in that slot; `node` models the same single slot.
    void* node = nullptr;
    // 0x004BAB3B: `call 0x004BAB80`; 0x004BAB43: `test eax,eax`; 0x004BAB46: `je 0x004BAB6D` - taken when the lookup fails.
    if (Script_QHashSymbolLookup_4BAB80(name, &node) == 0)
    {
        // 0x004BAB45: `push esi` (the name, and the %s vararg) / 0x004BAB6D: `push 0x005476FC` ("Undefined symbol: '%s'")
        // / 0x004BAB72: `call 0x004AF7B0`. The last push is the first argument, so the format is argument 1 and the name
        // is the vararg. 0x004BAB7A: `jmp 0x004BAB7A` - retail's terminal spin, preserved.
        Debug_Error("Undefined symbol: '%s'", name);
        for (;;) {}
    }
    // 0x004BAB48: `mov ecx,[0x00A43E88]` (the symbol table) / 0x004BAB4E: `push ecx` / 0x004BAB4F: `call 0x004C0460`;
    // the second argument is the pushed name word from 0x004BAB45.
    Script_QHashNodeRemove_4C0460(g_a43e88, name);
    // 0x004BAB54: `mov edx,[esp+0x10]` - the argument word, now holding the lookup's out value / 0x004BAB59: `call 0x004BAA90`
    Script_QHashNodeClearRef_4BAA90(node);
    // 0x004BAB5E: `mov eax,[esp+0x14]` - the same slot again / 0x004BAB63: `call 0x004B0D10`
    Mem_DeAllocFn(node);
    // 0x004BAB68: `add esp,0x10` is the cdecl cleanup for the four argument dwords this body pushed, emitted by the
    // compiler rather than written by hand. 0x004BAB6B: `pop esi` / 0x004BAB6C: `ret`.
}

} // extern "C"
