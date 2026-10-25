// Native: Script_QHashNodeRelease_4BACA0 (behavior-derived placeholder name; retail symbol unknown; it is the single node-release worker of the script symbol table, called only by 0x004BD380). retail VA 0x004BACA0-0x004BACC5 (38 bytes, 1 block, 0 edges, 13 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a ret) over the exact image; the extent is bounded by the `ret` at 0x004BACC5, and ten alignment nops `90` x10 follow it (next entry 0x004BACD0, which is Script_QHashSymbolTableInit). Exact bytes 56 8b 74 24 08 56 e8 b5 4c ff ff 56 68 b0 3e a4 00 e8 5a 4c ff ff a1 8c 3e a4 00 83 c4 0c 48 a3 8c 3e a4 00 5e c3 = `push %esi` / `mov esi,[esp+8]` / `push %esi` / `call 0x004AF960` / `push %esi` / `push $0x00A43EB0` / `call 0x004AF910` / `mov %eax,[0x00A43E8C]` / `add $0xc,%esp` / `dec %eax` / `mov [0x00A43E8C],%eax` / `pop %esi` / `ret`. Every call displacement resolves as v+5+rel32: 0x004BACAB + 0xFFFF4CB5 = 0x004AF960; 0x004BACB1 + 0x00004C5A = 0x004AF910. Call census from the image's 0xE8 enumeration: 0x004BD380 calls this body and nothing else does.
// Behavior/CFG: one cdecl pointer argument (the node at [esp+8] after `push %esi`), no frame, one block, no branch. Unlinks the node from its current list (0x004AF960), re-inserts it at the tail of the free list 0x00A43EB0 (0x004AF910 - the FIRST argument is the LAST push, so the head is &g_a43eb0 and the node is the second argument), then decrements the live count g_a43e8c with an explicit load/decrement/store (`mov eax,[..]` / `dec eax` / `mov [..],eax`) rather than a read-modify-write instruction, and returns nothing. 0x004BACBB `add $0xc,%esp` is the cdecl cleanup for the three argument dwords this body pushed.
// Status/limits: TRANSCRIBED; LINKABLE (its two callees 0x004AF960 Sys_UnlinkListNode_4AF960 and 0x004AF910 Script_QHashListInsert and its two globals g_a43eb0 / g_a43e8c are all tree natives, so it adds no new dependency); STRUCTURALLY_VERIFIED against the 13-instruction listing (1 block, 0 edges); runtime UNOBSERVED. THE NAME IS A PLACEHOLDER derived from the measured callee behavior (a node is unlinked, returned to the free list and the live count drops) and it must be revisited when the symbol table is understood as a whole. The node's interior layout is not interpreted here: retail only moves the 12-byte record and touches no field of it, so no field is named. Domain: script/, because both callees and both globals belong to the script symbol table at 0x00A43E88/0x00A43EB0 and its owner native_4bacd0.cpp.

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
extern "C" std::int32_t g_a43e8c;                                // 0x00A43E8C, live count

// 0x004AF960: unlinks the node from whichever list currently owns it (sys/native_4af960.cpp, manifest alias HeapList_Remove).
void* NATIVE_DECL Sys_UnlinkListNode_4AF960(void* node);
// 0x004AF910: inserts the node at the tail of the given list (script/native_4af910.cpp).
void NATIVE_DECL Script_QHashListInsert(void* head, void* node);

void NATIVE_DECL Script_QHashNodeRelease_4BACA0(void* node)
{
    // 0x004BACA5: `push esi` (the node) / 0x004BACA6: `call 0x004AF960` - one argument, so the call is (node).
    (void)Sys_UnlinkListNode_4AF960(node);
    // 0x004BACAB: `push esi` (the node, second argument) / 0x004BACAC: `push 0x00A43EB0` (the head, FIRST argument,
    // because it is the last push) / 0x004BACB1: `call 0x004AF910`.
    Script_QHashListInsert(&g_a43eb0, node);
    // 0x004BACB6: `mov eax,[0x00A43E8C]` / 0x004BACBE: `dec eax` / 0x004BACBF: `mov [0x00A43E8C],eax` - the retail
    // performs an explicit load, decrement and store; the add esp at 0x004BACBB between them is stack cleanup only.
    g_a43e8c = g_a43e8c - 1;
    // 0x004BACC4: `pop esi` / 0x004BACC5: `ret`. EAX holds the decremented count and no caller consumes it.
}

} // extern "C"
