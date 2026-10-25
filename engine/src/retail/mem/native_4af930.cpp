// Native: HeapList_PopFront_4AF930; retail VA 0x004AF930 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004AF930-0x004AF954 (37 bytes, 3 blocks, 2 edges, 13 instructions), bytes re-read from the image (8b 54 24 04 8b 02 8d 4a 04 39 c8 75 03 33 c0 c3 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; sole caller 0x004BABE5 (0x004BABB0) pushes the list header address, matching the one-argument cdecl shape. No callees (leaf).
// Behavior/CFG: pops the front node of the intrusive doubly-linked list whose header is the caller's container. Empty is `header[0] == &header[4]` (0x004AF934/0x004AF936/0x004AF939), which returns 0. Otherwise node = header[0] (0x004AF934), header[0] = node[0] (0x004AF940/0x004AF942), the new front node's back link node[0][1] = header (0x004AF944), and the popped node's two links are cleared with dword stores (0x004AF947/0x004AF94E); EAX returns the popped node. This is the exact counterpart of the tail insert at 0x004AF8F0 (`HeapList_Insert`), which stores `node[0] = container + 4` and `node[1] = *(container + 8)`.
// Status/limits: BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the instruction listing (3 blocks, 2 edges); runtime UNOBSERVED. Link: no callees and no globals. Domain note: filed under `mem` because it only manipulates an intrusive list header and its nearest VAs (0x004AF8F0, 0x004AF960) live in `mem/heap_list.cpp`; its only caller is the script symbol table (0x004BABB0), so the Lead may relocate the file without touching its bytes. The empty test compares header[0] against the address of header[4], not against header itself - kept literal.
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

void* NATIVE_DECL HeapList_PopFront_4AF930(void* const container)
{
    NATIVE_LOG_ENTER("HeapList_PopFront_4AF930", "0x004AF930");
    std::uint8_t* const header = static_cast<std::uint8_t*>(container);          // 0x004AF930: mov 0x4(%esp),%edx
    std::uint8_t* const anchor = header + 4;                                     // 0x004AF936: lea 0x4(%edx),%ecx
    std::uint8_t* const node = *reinterpret_cast<std::uint8_t**>(header);        // 0x004AF934: mov (%edx),%eax
    if (node == anchor) {                                                        // 0x004AF939: cmp %ecx,%eax (empty list)
        return nullptr;                                                          // 0x004AF93D: xor %eax,%eax
    }
    std::uint8_t* const next = *reinterpret_cast<std::uint8_t**>(node);          // 0x004AF940: mov (%eax),%ecx
    *reinterpret_cast<std::uint8_t**>(header) = next;                            // 0x004AF942: mov %ecx,(%edx)
    *reinterpret_cast<std::uint8_t**>(next + 4) = header;                        // 0x004AF944: mov %edx,0x4(%ecx)
    *reinterpret_cast<std::uint32_t*>(node + 4) = 0;                             // 0x004AF947: movl $0x0,0x4(%eax)
    *reinterpret_cast<std::uint32_t*>(node) = 0;                                 // 0x004AF94E: movl $0x0,(%eax)
    return node;                                                                 // 0x004AF954: ret, EAX = popped node
}

} // extern "C"

#undef NATIVE_DECL
