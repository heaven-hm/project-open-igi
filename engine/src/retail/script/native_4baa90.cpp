// Native: Script_QHashNodeClearRef_4BAA90 (behavior-derived placeholder name; retail symbol unknown). retail VA 0x004BAA90-0x004BAAB4 (37 bytes, 2 blocks, 3 edges, 16 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x004BAA90 plus capstone 5.0.7 aligned from the verified entry; both agree on 16 instructions. Exact bytes 8b 0d 90 3e a4 00 33 d2 8b 01 3b c2 74 16 56 8b 74 24 08 39 71 08 75 03 89 51 08 8b c8 8b 00 3b c2 75 f0 5e c3 = `mov ecx,[0x00A43E90]` / `xor edx,edx` / `mov eax,[ecx]` / `cmp eax,edx` / `je 0x004BAAB4` / `push %esi` / `mov esi,[esp+8]` / `cmp [ecx+8],esi` / `jne 0x004BAAB` / `mov [ecx+8],edx` / `mov ecx,eax` / `mov eax,[eax]` / `cmp eax,edx` / `jne 0x004BAAA3` / `pop %esi` / `ret`. The ret is at 0x004BAAB4 and the extent is 37 bytes; 0x004BAAB5 onward is `nop` alignment padding. The body contains NO call instructions at all, so it has no callees and adds no link dependency; its only callee-shaped references are the list global 0x00A43E90. Call census from the whole image's 0xE8 enumeration: it is called from 0x004BAB59 in Script_QHashSymbolRemove_4BAB30 and from 0x004B8945 in the 0x004B8920 cluster.
// Behavior/CFG: one argument (a value pointer), cdecl, no frame, and the only loop in the chain. It walks the live symbol-node list whose header 0x00A43E90 is owned by 0x004BACD0 (native_4bacd0.cpp) and clears the +8 dword of every node that holds the argument. The node shape is not invented here: native_4babb0.cpp records the 12-byte node records threaded on 0x00A43E90 with the symbol lookup's out value stored at node+8, and this body's `cmp [ecx+8],esi` / `mov [ecx+8],edx` touches exactly that field. THE WALK IS ASYMMETRIC AND THE ASYMMETRY IS PRESERVED: the guard is `eax = [first]; if (eax == 0) return;` - it tests the FIRST NODE'S OWN FORWARD LINK BEFORE examining that node - so the head is examined only when a second node exists, the LAST node of the list is never examined at all, and a single-node list is skipped entirely. A conventional `for (node = head; node; node = node->next)` loop would examine both the head and the tail and would therefore be a behavior change, not a cleanup. `xor edx,edx` is the null written into the matching field, and the loop's back edge is 0x004BAAB1 -> 0x004BAAA3. The `push %esi`/`pop %esi` pair is retail saving the argument across the loop and is handled by the compiler here.
// Status/limits: TRANSCRIBED; LINKABLE (no callees; its only reference is the tree global g_a43e90 defined by 0x004BACD0); STRUCTURALLY_VERIFIED against the 16-instruction listing (2 blocks, 3 edges: the `je` exit, the fallthrough, and the loop back edge); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. The SymbolSentinel declaration below repeats native_4babb0.cpp's type token for token rather than inventing a second shape for the same header. THE NAME IS A PLACEHOLDER derived from measured behavior (it clears a stored reference, it does not unlink a node - the unlink is 0x004AF960 elsewhere); what the +8 value means semantically is not interpreted here. Domain: script/, because the list it walks is the script symbol table's live node list.

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

// Sentinel list header owned by Script_QHashSymbolTableInit (native_4bacd0.cpp); same definition there and in
// native_4babb0.cpp.
struct SymbolSentinel {
    std::uint8_t* field_first;                                   // +0
    std::uint8_t* field_middle;                                  // +4
    std::uint8_t* field_back;                                    // +8
};
extern "C" SymbolSentinel g_a43e90;                              // 0x00A43E90/94/98, the live node list

void NATIVE_DECL Script_QHashNodeClearRef_4BAA90(void* const value)
{
    NATIVE_LOG_ENTER("Script_QHashNodeClearRef_4BAA90", "0x004BAA90");
    // 0x004BAA90: `mov ecx,[0x00A43E90]` - the first node of the live list.
    std::uint8_t* current = g_a43e90.field_first;
    // 0x004BAA96: `xor edx,edx` (the null written below) / 0x004BAA98: `mov eax,[ecx]` - the first node's own forward
    // link. 0x004BAA9A/0x004BAA9C: `cmp eax,edx` / `je 0x004BAAB4`. RETAIL RETURNS HERE WITHOUT EXAMINING ANY NODE: the
    // guard reads the head's forward link before the head is compared, so the tail is never examined and a one-node list
    // is skipped entirely. Preserved rather than normalised into a conventional list walk.
    std::uint8_t* next = *reinterpret_cast<std::uint8_t**>(current);
    if (next == nullptr)
    {
        return;                                                                    // 0x004BAAB4
    }
    do
    {
        if (*reinterpret_cast<void**>(current + 8) == value)                       // 0x004BAAA3/0x004BAAA6
        {
            *reinterpret_cast<void**>(current + 8) = nullptr;                      // 0x004BAAA8: `mov edx,0x8(%ecx)`
        }
        current = next;                                                            // 0x004BAAAB: `mov ecx,eax`
        next = *reinterpret_cast<std::uint8_t**>(current);                          // 0x004BAAAD: `mov eax,[eax]`
    } while (next != nullptr);                                                     // 0x004BAAB1: `jne 0x004BAAA3`
    // 0x004BAAB3: `pop %esi` / 0x004BAAB4: `ret`.
}

} // extern "C"
