// Native: Sys_InitListHead_4F1D60; retail VA 0x004F1D60 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1D60 (see the per-instruction comments; 8 instructions, 1 blocks, 0 edges).
// Tools/evidence: radare2 6.2.0 af/afij/pdf and NASM ndisasm over the exact bytes; it stores twice into the head, once into head+4 and once into head+8
// Behavior/CFG: initialises a four-field head: context at +0x0C, head+4 at +0x00, the head address at +0x08 and zero at +4. Straight-line, no branch, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The object's type is not established beyond the offsets this body reads.
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

void NATIVE_DECL Sys_InitListHead_4F1D60(void* head, std::int32_t context)
{
    NATIVE_LOG_ENTER("Sys_InitListHead_4F1D60", "0x004F1D60");
    std::uint8_t* const base = static_cast<std::uint8_t*>(head);          // 0x4F1D60: mov 0x4(%esp),%eax
    *reinterpret_cast<std::int32_t*>(base + 0x0CU) = context;            // 0x4F1D68: mov %ecx,0xC(%eax)
    std::uint8_t* const self_next = base + 0x04U;                        // 0x4F1D6B: lea 0x4(%eax),%ecx
    *reinterpret_cast<std::uint8_t**>(base + 0x00U) = self_next;         // 0x4F1D6E: mov %ecx,(%eax)
    *reinterpret_cast<void**>(base + 0x08U) = base;                      // 0x4F1D70: mov %eax,0x8(%eax)
    *reinterpret_cast<std::uint32_t*>(self_next) = 0U;                   // 0x4F1D73: movl $0x0,(%ecx)
}

} // extern "C"

#undef NATIVE_DECL
