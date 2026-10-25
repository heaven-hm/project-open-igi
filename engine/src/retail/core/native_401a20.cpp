// Native: TaskType_Clear_401A20; retail VA 0x00401A20 (0x00401A20-0x00401A51, 50 bytes, 3 blocks, 3 edges, 15 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 byte+instruction listing, bytes 8b 44 24 04 56 25 ff ff 00 00 8d 34 40 c1 e6 04 8a 86 04 5f af 00 84 c0 74 0f 8d 86 ec 5e af 00 50 e8 2a f1 0a 00 83 c4 04 c6 86 05 5f af 00 00 5e c3); radare2 6.2.0 `af/afij` for the CFG numbers; 279 direct call sites in .text, a set disjoint from 0x00401900's 279 sites.
// Behavior/CFG: masks the class id to its low 16 bits and addresses the 0x30-byte type slot at 0x00AF5EE0 + id*0x30; when the +0x24 class-task-list flag is set it calls 0x004B0B70 with &slot+0x0C (which releases descriptor[0] and stores null back) and then clears the +0x25 in-use byte. The +0x24 flag is not cleared here, and there is no return value.
// Status/limits: TRANSCRIBED (static only); runtime UNOBSERVED. Depends on QFile_ReleaseBlock_4B0B70 (0x004B0B70, engine/src/retail/qfile/native_4b0b70.cpp), which is already in the igi source list; nothing else is needed.
//
// Retail statement order preserved: argument load (0x00401A20), the low-word mask (0x00401A25), the slot scaling
// (0x00401A2A/0x00401A2D), the +0x24 flag test (0x00401A30-0x00401A38), the release call inside the branch
// (0x00401A3A-0x00401A46) and the +0x25 in-use clear after the branch (0x00401A49) are at their retail positions.
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

// Callee 0x004B0B70 (engine/src/retail/qfile/native_4b0b70.cpp): one pointer argument, the address of the slot's
// +0x0C indexed-block descriptor. It releases descriptor[0] through 0x004B0D10 and stores null at descriptor[0];
// 0x00401A46 cleans the single argument with `add $0x4,%esp`.
void NATIVE_DECL QFile_ReleaseBlock_4B0B70(void** block);

void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id)
{
    // 0x00401A20-0x00401A2D: the argument reaches the callee in a dword stack slot and only its low word is used
    // (`and $0xffff,%eax` at 0x00401A25); the 0x30-byte slot is 0x00AF5EE0 + id*0x30 (`lea (%eax,%eax,2),%esi;
    // shl $0x4,%esi`). The retail also pushes %esi here and pops it at 0x00401A50; that is register preservation,
    // not a side effect.
    igi::retail::core::TaskTypeSlot& slot = ::QTask_TypeTable[class_id];

    // 0x00401A30-0x00401A38: `mov 0xaf5f04(%esi),%al; test %al,%al; je 0x00401A49`. When the flag is zero the
    // branch is skipped and only the in-use clear runs.
    if (slot.has_task_list != 0U)
    {
        // 0x00401A3A-0x00401A46: `lea 0xaf5eec(%esi),%eax; push %eax; call 0x004B0B70; add $0x4,%esp`. The
        // descriptor starts at slot+0x0C, so the callee releases descriptor[0] (the pool base) and stores null
        // there. The +0x24 flag is left set: retail clears only the in-use byte below.
        QFile_ReleaseBlock_4B0B70(reinterpret_cast<void**>(slot.task_list));
    }

    // 0x00401A49-0x00401A51: `movb $0x0,0xaf5f05(%esi); pop %esi; ret`. The in-use clear happens after the
    // release, so a teardown that frees the block also leaves the slot reusable in this order.
    slot.in_use = 0U;
}

} // extern "C"

#undef NATIVE_DECL
