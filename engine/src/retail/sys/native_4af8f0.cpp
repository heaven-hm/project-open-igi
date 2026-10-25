// Native: Sys_InsertListTail_4AF8F0; retail VA 0x004AF8F0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004AF8F0-0x004AF90B (28 bytes, 1 block, 0 edges, 10 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the exact bytes; no call, no global, two arguments.
// Behavior/CFG: links a node onto the tail of a four-field head. The node's back pointer at +0x04 takes the head's tail pointer from +0x08; the node's forward pointer at +0x00 takes the address of the head's own +0x04 field; the previous tail's forward pointer is rewritten to the node; and the head's tail pointer becomes the node. Straight-line, no branch, no call, no global - it is the insert half of the list pair whose initialiser is 0x004F1D60.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The list layout is what this body and 0x004F1D60 jointly establish: +0x00 forward, +0x04 back, +0x08 tail, +0x0C context; the node type beyond its first two fields is not established.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::sys {

void NATIVE_DECL Sys_InsertListTail_4AF8F0(void* head, void* node)
{
    std::uint8_t* const base = static_cast<std::uint8_t*>(head);           // 0x4AF8F0: mov 0x4(%esp),%eax
    std::uint8_t* const item = static_cast<std::uint8_t*>(node);           // 0x4AF8F4: mov 0x8(%esp),%ecx
    std::uint8_t* previous = *reinterpret_cast<std::uint8_t**>(base + 0x08U); // 0x4AF8F8: mov 0x8(%eax),%edx
    *reinterpret_cast<std::uint8_t**>(item + 0x04U) = previous;           // 0x4AF8FB: mov %edx,0x4(%ecx)
    std::uint8_t* const link = base + 0x04U;                               // 0x4AF8FE: lea 0x4(%eax),%edx
    *reinterpret_cast<std::uint8_t**>(item) = link;                        // 0x4AF901: mov %edx,(%ecx)
    previous = *reinterpret_cast<std::uint8_t**>(base + 0x08U);            // 0x4AF903: mov 0x8(%eax),%edx
    *reinterpret_cast<std::uint8_t**>(previous) = item;                    // 0x4AF906: mov %ecx,(%edx)
    *reinterpret_cast<std::uint8_t**>(base + 0x08U) = item;                // 0x4AF908: mov %ecx,0x8(%eax)
}                                                                         // 0x4AF90B: ret

} // namespace igi::retail::sys

#undef NATIVE_DECL
