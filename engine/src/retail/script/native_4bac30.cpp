// Native: Script_QHashSymbolPoolRefill_4BAC30; retail VA 0x004BAC30 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BAC30-0x004BAC91 (98 bytes, 3 blocks, 2 edges, 33 instructions), bytes re-read from the image (53 8b 5c 24 08 56 57 8d 44 5b 03 6a 04 c1 e0 02 50 e8 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; sole caller 0x004BABD7 (0x004BABB0) pushes 0x400. Shares the sentinel headers with 0x004BACD0 (native_4bacd0.cpp) and the list primitives 0x004AF8F0 / 0x004AF930.
// Behavior/CFG: cdecl, one argument (count). Allocates `(count * 3 + 3) * 4` bytes through Mem_Alloc(0x004B0C60) with the 4-byte flag (0x004BAC37-0x004BAC41), clears the block's first two dwords (0x004BAC4E/0x004BAC55), inserts the block at the tail of the sentinel list 0x00A43EA0 (0x004BAC5B), stores block + 0x0C at block + 8 (0x004BAC63/0x004BAC68), then for each of `count` 12-byte entries clears the entry's first two dwords and inserts it at the tail of the sentinel list 0x00A43EB0 (0x004BAC6D-0x004BAC8C). Every clear is a pair of dword stores that 0x004AF8F0 immediately overwrites with the link words - the retail executes them, so they are kept.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the instruction listing (3 blocks, 2 edges); runtime UNOBSERVED. Link: Mem_Alloc (transcribed, engine/src/retail/mem/mem_alloc.cpp), HeapList_Insert 0x004AF8F0 (transcribed, mem/heap_list.cpp), sentinel globals g_a43ea0 / g_a43eb0 (defined in native_4bacd0.cpp; the local SymbolSentinel declaration below repeats that file's type token for token so both translation units name one entity). Return: the retail leaves EAX at the last HeapList_Insert result and its only caller ignores it, so the port declares void.
#include "dev_log.hpp"
#include <cstdint>

#include "igi/retail/mem/mem_alloc.hpp"

extern "C" void* Sys_InsertListTail_4AF8F0(void* head, void* node);   // 0x004AF8F0, engine/src/retail/sys/native_4af8f0.cpp

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
extern "C" SymbolSentinel g_a43ea0;                              // 0x00A43EA0/A4/A8
extern "C" SymbolSentinel g_a43eb0;                              // 0x00A43EB0/B4/B8
void NATIVE_DECL Script_QHashSymbolPoolRefill_4BAC30(std::int32_t count)
{
    NATIVE_LOG_ENTER("Script_QHashSymbolPoolRefill_4BAC30", "0x004BAC30");
    // 0x004BAC37-0x004BAC41: Mem_Alloc((count * 3 + 3) * 4, 4).
    std::uint8_t* const block = static_cast<std::uint8_t*>(
        igi::retail::mem::Mem_Alloc((count * 3 + 3) * 4, 4));                            // 0x004BAC46
    *reinterpret_cast<std::uint32_t*>(block + 4) = 0;                                    // 0x004BAC4E
    *reinterpret_cast<std::uint32_t*>(block) = 0;                                        // 0x004BAC55
    Sys_InsertListTail_4AF8F0(&g_a43ea0, reinterpret_cast<void**>(block));       // 0x004BAC5B
    std::uint8_t* entry = block + 0x0C;                                                  // 0x004BAC63: lea 0xc(%edi),%esi
    *reinterpret_cast<std::uint8_t**>(block + 8) = entry;                                // 0x004BAC68: mov %esi,0x8(%edi)
    if (count <= 0) {                                                                    // 0x004BAC66/0x004BAC6B: jle
        return;
    }
    do {
        *reinterpret_cast<std::uint32_t*>(entry + 4) = 0;                                // 0x004BAC73
        *reinterpret_cast<std::uint32_t*>(entry) = 0;                                    // 0x004BAC7A
        Sys_InsertListTail_4AF8F0(&g_a43eb0, reinterpret_cast<void**>(entry));   // 0x004BAC80
        entry += 0x0C;                                                                   // 0x004BAC88: add $0xc,%esi
        --count;                                                                         // 0x004BAC8B: dec %ebx
    } while (count != 0);                                                                // 0x004BAC8C: jne
}

} // extern "C"

#undef NATIVE_DECL
