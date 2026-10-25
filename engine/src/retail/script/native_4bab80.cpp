// Native: Script_QHashSymbolLookup_4BAB80; retail VA 0x004BAB80 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BAB80-0x004BAB99 (26 bytes, 1 block, 0 edges, 9 instructions), bytes re-read from the image (8b 04 24 8b 4c 24 08 8b 15 88 3e a4 00 50 51 52 e8 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; callers: 0x004B893B (0x004B8CE0's cluster), 0x004BAB3B, 0x004BABBA, 0x004BCD D9 (0x004BCD90) and 0x004BEB69 - each pushes two arguments.
// Behavior/CFG: cdecl forwarder with two stack arguments ([esp+4] name, [esp+8] node out). It loads the symbol-table pointer stored at the retail global 0x00A43E88 (0x004BAB88: `mov 0xa43e88,%edx`, the pointer value, not its address) and calls 0x004C0620 with (table, node out, name) - the pushes at 0x004BAB8E-0x004BAB91 place the name first so it becomes the third argument, and the twelve-byte cleanup at 0x004BAB96 confirms three cdecl arguments. It returns 0x004C0620's result unchanged.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; STRUCTURALLY_VERIFIED against the instruction listing only. Link: needs Script_QHashSymbolLookupRecord_4C0620 (transcribed in this pass) and the global g_a43e88, already defined in native_4bacd0.cpp (0x004BACD0 creates the table with 4999 buckets). The forwarder adds no logic of its own; the record fields it reaches through 0x004C0620 are not interpreted here.
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

// 0x00A43E88 - the script symbol table, created by 0x004BACD0 (see native_4bacd0.cpp).
extern "C" void* g_a43e88;
std::int32_t NATIVE_DECL Script_QHashSymbolLookupRecord_4C0620(void* table, void** out_node, const char* name);

std::int32_t NATIVE_DECL Script_QHashSymbolLookup_4BAB80(const char* name, void** out_node)
{
    NATIVE_LOG_ENTER("Script_QHashSymbolLookup_4BAB80", "0x004BAB80");
    // 0x004BAB88-0x004BAB91: 0x004C0620(g_a43e88, node out, name).
    return Script_QHashSymbolLookupRecord_4C0620(g_a43e88, out_node, name);
}

} // extern "C"

#undef NATIVE_DECL
