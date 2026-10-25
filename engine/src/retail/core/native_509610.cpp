// Native: AIGraph_Handler_509610; retail VA 0x00509610 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509610-0x0050964A (59 bytes, 3 blocks, 17 instructions).
// Tools/evidence: build/retail-dis.txt (GNU objdump) 0x00509610-0x0050964A; radare2 6.2.0 afij (59 bytes, 3 blocks, 17 instructions); the table base is 0x00A976E0, which is QTask_HandlerTable + 2 * 0x180 entries (phase 2) per core/task_types.hpp; linked by the AIGraph registrar 0x00508FA0 at 0x00509030 (0x00401530 event 2, the no-context link).
// Behavior/CFG: three blocks, no loop. Frees the buffer word at object+0xB8 through 0x004B0D10 when it is non-zero and clears the word, then resolves the object's parent type with the 0x004F9210 getter and calls the phase-2 class handler the table holds for it. Retail calls the table entry without a null test (0x00509644 `call *%eax`), and that absence is preserved.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 17-instruction listing; runtime UNOBSERVED. 0x004B0D10 and 0x004F9210 are transcribed in the tree (Mem_DeAllocFn, Graph_Get_4F9210), so this file has no missing callee of its own.
#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void Mem_DeAllocFn(void* pointer);                       // 0x004B0D10, engine/src/retail/mem/native_4b0d10.cpp
std::uint16_t Graph_Get_4F9210();                         // 0x004F9210, engine/src/retail/core/native_4f9210.cpp

void NATIVE_DECL AIGraph_Handler_509610(void* object)
{
    NATIVE_LOG_ENTER("AIGraph_Handler_509610", "0x00509610");
    std::uint8_t* const fields = static_cast<std::uint8_t*>(object);

    void* const buffer = *reinterpret_cast<void**>(fields + 0xB8U);            // 0x00509615: mov 0xb8(%esi),%eax
    if (buffer != nullptr)                                                    // 0x0050961B/0x0050961D: test %eax,%eax; je 0x509632
    {
        Mem_DeAllocFn(buffer);                                                // 0x00509620: push %eax; call 0x4b0d10
        *reinterpret_cast<std::uint32_t*>(fields + 0xB8U) = 0U;               // 0x00509628: movl $0x0,0xb8(%esi)
    }

    // 0x00509632: call 0x4f9210; 0x00509637: and $0xffff,%eax. The mask reproduces retail leaving the getter's upper EAX bits dirty; a
    // std::uint16_t return already zero-extends, so the mask is unobservable in the port.
    std::uint32_t const parent = Graph_Get_4F9210();
    // 0x0050963D: mov 0xa976e0(,%eax,4),%eax - the phase-2 table entry for the parent class.
    igi::retail::core::FrameHandler const handler =
        ::QTask_HandlerTable[2U * igi::retail::core::task_phase_stride + parent];
    handler(object);                                                          // 0x00509644: call *%eax
}                                                                             // 0x0050964A: ret
} // extern "C"

#undef NATIVE_DECL
