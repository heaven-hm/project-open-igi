// Native: AIGraph_Handler_509570; retail VA 0x00509570 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509570-0x00509606 (151 bytes, 1 block, 28 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00509570-0x00509606; radare2 6.2.0 afij (151 bytes, 1 block, 28 instructions); linked by the AIGraph registrar 0x00508FA0 at 0x0050900E (0x00401400 event 0).
// Behavior/CFG: one block, no branch. Calls 0x004F90B0(object, arg2), then initialises the object's parameter block 0x80..0xBC with the retail dword pairs (each double is stored as two 32-bit immediates, low word first): +0x80 = 1.0, +0xA0 = 1.0, +0x88 = 2.0, +0x90 = 3.0, +0x98 = 0.3, +0xA8 = 0.05, +0xB0 and +0xB4 = -1, +0xB8 = 0, +0xBC = 0 (byte).
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 28-instruction listing; runtime UNOBSERVED. Link-blocked on 0x004F90B0 (not transcribed); its prototype is inferred from this call site. The dword stores are transcribed individually, in retail's order, because retail writes some halves separately (0x005095B1 writes +0x98 before +0x8C, etc.).
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

// 0x004F90B0: the graph node field initialiser (168 bytes, 3 blocks, 60 instructions; calls the allocation pair 0x004B0D60/0x004B0D10).
// NOT transcribed yet - declared only.
void NATIVE_DECL GraphNode_Init_4F90B0(void* object, void* arg2);

void NATIVE_DECL AIGraph_Handler_509570(void* object, void* arg2)
{
    NATIVE_LOG_ENTER("AIGraph_Handler_509570", "0x00509570");
    std::uint8_t* const fields = static_cast<std::uint8_t*>(object);

    GraphNode_Init_4F90B0(object, arg2);                                      // 0x0050957B: push %eax (arg2); push %esi; call 0x4F90B0

    *reinterpret_cast<std::uint32_t*>(fields + 0x80U) = 0U;                   // 0x00509587: mov %eax,0x80(%esi)  (eax = 0)
    *reinterpret_cast<std::uint32_t*>(fields + 0xA0U) = 0U;                   // 0x0050958D: mov %eax,0xa0(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0x84U) = 0x3FF00000U;          // 0x00509593: mov %ecx,0x84(%esi)  (ecx = 0x3ff00000)
    *reinterpret_cast<std::uint32_t*>(fields + 0xA4U) = 0x3FF00000U;          // 0x00509599: mov %ecx,0xa4(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0x88U) = 0U;                   // 0x005095A5: mov %eax,0x88(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0x90U) = 0U;                   // 0x005095AB: mov %eax,0x90(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0x98U) = 0x33333333U;          // 0x005095B1: movl $0x33333333,0x98(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0xA8U) = 0x9999999AU;          // 0x005095BB: movl $0x9999999a,0xa8(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0x8CU) = 0x40000000U;          // 0x005095C5: movl $0x40000000,0x8c(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0x94U) = 0x40080000U;          // 0x005095CF: movl $0x40080000,0x94(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0x9CU) = 0x3FD33333U;          // 0x005095D9: movl $0x3fd33333,0x9c(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0xACU) = 0x3FA99999U;          // 0x005095E3: movl $0x3fa99999,0xac(%esi)
    *reinterpret_cast<std::uint8_t*>(fields + 0xBCU) = 0U;                    // 0x005095ED: mov %al,0xbc(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0xB0U) = 0xFFFFFFFFU;          // 0x005095F3: mov %ecx,0xb0(%esi)  (ecx = -1)
    *reinterpret_cast<std::uint32_t*>(fields + 0xB4U) = 0xFFFFFFFFU;          // 0x005095F9: mov %ecx,0xb4(%esi)
    *reinterpret_cast<std::uint32_t*>(fields + 0xB8U) = 0U;                   // 0x005095FF: mov %eax,0xb8(%esi)
}                                                                             // 0x00509606: ret
} // extern "C"

#undef NATIVE_DECL
