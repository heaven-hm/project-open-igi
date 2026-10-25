// Native: Script_QHashSymbolNodeCreate_4BABB0; retail VA 0x004BABB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BABB0-0x004BAC21 (114 bytes, 7 blocks, 8 edges, 35 instructions), bytes re-read from the image (8b 4c 24 04 8d 44 24 04 50 51 e8 ... 83 c4 08 85 c0 74 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; this ret, and the two jumps to it, close at 0x004BAC1E and 0x004BAC1F. Called twice by 0x004B9120 and twice by 0x004B8CE0, so it sits on both expression-evaluator paths.
// Behavior/CFG: cdecl, one argument (name). Looks the name up through 0x004BAB80(name, &out) - the second argument is the address of the incoming argument word itself (0x004BABB4 `lea 0x4(%esp),%eax`), so the lookup overwrites the name word with its out value and the caller's original pointer is consumed. A miss returns 0 (0x004BABC4/0x004BAC1F). On a hit: if the free sentinel list 0x00A43EB0 is empty ([0x00A43EB0] == 0x00A43EB4, 0x004BABC6/0x004BABD0) it refills it with 1024 entries via 0x004BAC30; it then pops one 12-byte node from that list through 0x004AF930 (0x004BABE5), stores the lookup out value at node + 8 (0x004BABEA/0x004BABF6 - this is the overwritten argument word, not the name), inserts the node at the front of the live sentinel list 0x00A43E90 with 0x004AF910 (0x004BABF9), increments the live counter 0x00A43E8C and raises the high-water counter 0x00A43EAC when the new count exceeds it with an unsigned compare (0x004BABFE-0x004BAC16), and returns the node (0x004BAC1B). The three pushes for 0x004AF930 and 0x004AF910 are cleaned together by the single `add $0xc` at 0x004BAC09.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the instruction listing (7 blocks, 8 edges); runtime UNOBSERVED. Link: needs 0x004BAB80 (transcribed), 0x004BAC30 (transcribed), 0x004AF930 (transcribed), 0x004AF910 (transcribed) and globals g_a43eb0 / g_a43e90 / g_a43e8c / g_a43eac (defined in native_4bacd0.cpp; the SymbolSentinel declaration below repeats that file's type token for token). What the stored node means - it is a 12-byte record whose +8 dword is the symbol lookup's out value and which is threaded on 0x00A43E90 - is not interpreted here; the retail states no meaning in this function.
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

// Sentinel list header owned by Script_QHashSymbolTableInit (native_4bacd0.cpp); same definition there.
struct SymbolSentinel {
    std::uint8_t* field_first;                                   // +0
    std::uint8_t* field_middle;                                  // +4
    std::uint8_t* field_back;                                    // +8
};
extern "C" SymbolSentinel g_a43eb0;                              // 0x00A43EB0/B4/B8, the free node list
extern "C" SymbolSentinel g_a43e90;                              // 0x00A43E90/94/98, the live node list
extern "C" std::int32_t g_a43e8c;                                // 0x00A43E8C, live count
extern "C" std::int32_t g_a43eac;                                // 0x00A43EAC, high-water count

std::int32_t NATIVE_DECL Script_QHashSymbolLookup_4BAB80(const char* name, void** out_node);
void NATIVE_DECL Script_QHashSymbolPoolRefill_4BAC30(std::int32_t count);
void* NATIVE_DECL HeapList_PopFront_4AF930(void* container);
void NATIVE_DECL Script_QHashListInsert(void* head, void* node);

void* NATIVE_DECL Script_QHashSymbolNodeCreate_4BABB0(const char* name)
{
    NATIVE_LOG_ENTER("Script_QHashSymbolNodeCreate_4BABB0", "0x004BABB0");
    void* lookup_value = nullptr;
    // 0x004BABB4-0x004BABBA: 0x004BAB80(name, &lookup_value), the out slot being the name's own argument word in the retail frame.
    std::int32_t const found = Script_QHashSymbolLookup_4BAB80(name, &lookup_value);
    if (found == 0) {                                                                   // 0x004BABC2-0x004BABC4
        return nullptr;                                                                 // 0x004BAC1F
    }

    // 0x004BABC6/0x004BABD0: refill when the free list is empty (its first link points at its own +4 anchor).
    if (g_a43eb0.field_first == reinterpret_cast<std::uint8_t*>(&g_a43eb0.field_middle)) {
        Script_QHashSymbolPoolRefill_4BAC30(0x400);                                     // 0x004BABD2-0x004BABD7
    }

    std::uint8_t* const node = static_cast<std::uint8_t*>(HeapList_PopFront_4AF930(&g_a43eb0));  // 0x004BABE5
    *reinterpret_cast<void**>(node + 8) = lookup_value;                                 // 0x004BABF6: mov %edx,0x8(%esi)
    Script_QHashListInsert(&g_a43e90, node);                                            // 0x004BABF9

    std::int32_t const count = g_a43e8c + 1;                                            // 0x004BAC0C: inc %eax
    g_a43e8c = count;                                                                   // 0x004BAC0F
    if (static_cast<std::uint32_t>(count) > static_cast<std::uint32_t>(g_a43eac)) {     // 0x004BAC0D/0x004BAC14: cmp/jbe (unsigned)
        g_a43eac = count;                                                               // 0x004BAC16
    }
    return node;                                                                        // 0x004BAC1B-0x004BAC1E
}

} // extern "C"

#undef NATIVE_DECL
