// Native: TaskType_Register; retail VA 0x00401900 (0x00401900-0x00401A10, 273 bytes, 18 blocks, 23 edges, 77 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: build/retail-dis.txt (GNU objdump i386 byte+instruction listing) for every instruction below; radare2 6.2.0 `aaa; afij` for the CFG numbers; level-1 registration capture docs/fidelity/evidence/0x00401900-retail-registration-order.json (279 records, ids 0..278).
// Behavior/CFG: scans the 0x30-byte type slots from 0x00AF5F05 in_use byte upward (bound 0x00AFA705 = 384 slots) and spins forever after Debug_Error("Unable to allocate task type") when full; on a slot writes in_use=1 (+0x25), object size (+0x04), name dword (+0x00), parent word (+0x08), inherited flags (+0x2C, 0x20000000 for parent 0x0181 else the parent slot's +0x2C); when the fourth stack slot is non-zero calls 0x004B0B00(&slot+0x0C, object_size, task_list_count, 0) and sets +0x24=1 else +0x24=0; then copies the parent's 0xFE handler-table entries (0x600-byte row stride) or zeroes them for 0x0181, calls 0x004014B0 in each arm, and returns the type id in AX (`mov %bp,%ax`).
// Status/limits: BUILD_VERIFIED TARGET - the same definition is committed in engine/src/retail/core/task_types.cpp, which the manifest record for 0x00401900 still names as the implementation; the owner must delete that definition, list this file in CMakeLists.txt and repoint the manifest record in the same change (rule 10.15 no-duplication). 32-bit target only: the name is stored as a 32-bit dword, exactly as 0x00401949 does. Retail statement order preserved: the slot scan, the error spin, the +0x25/+0x04/+0x00 stores, the +0x08 word, the +0x2C branch, the fourth-slot branch at 0x00401982, the 0x0181 re-test at 0x004019AD, the two separate row loops (0x004019B4-0x004019DD copy, 0x004019EB-0x00401A03 zero) and the two separate 0x004014B0 calls (0x004019DF, 0x00401A05) are all at their retail positions.
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

// Callee 0x004AF7B0 (Debug_Error); the error path pushes the literal and cleans the stack with `add $0x4,%esp`.
std::int32_t NATIVE_DECL Debug_Error(const char* format, ...);

// Callee 0x004B0B00 (Mem_AllocIndexedBlock); 0x00401995 is called with (slot+0x0C, object_size, count, 0).
std::int32_t NATIVE_DECL Mem_AllocIndexedBlock(std::int32_t* descriptor, std::int32_t element_size,
                                              std::int32_t count, std::int32_t alignment);

// task_types.hpp declares this native inside its extern "C" block at line 74, so the definition keeps
// C linkage here too: all 279 calling sites reference the unmangled TaskType_Register symbol.
std::uint16_t NATIVE_DECL TaskType_Register(const char* name, std::uint32_t object_size,
                                            std::uint16_t parent_type,
                                            std::uint32_t task_list_count)
{
    NATIVE_LOG_ENTER("TaskType_Register", "0x00401900");
    // 0x00401903-0x00401916: eax starts at the in_use byte of type 0 and advances 0x30 per slot; the loop keeps
    // going while the in_use byte is set and the slot address stays below 0x00AFA705.
    std::uint32_t type_id = 0U;
    for (; type_id < igi::retail::core::task_type_slot_count; ++type_id)
    {
        if (::QTask_TypeTable[type_id].in_use == 0U)          // 0x00401908: cmpb $0x0,(%eax)
        {
            break;                                            // 0x0040190B: je 0x00401927
        }
    }
    if (type_id >= igi::retail::core::task_type_slot_count)   // 0x00401911: cmp $0xafa705,%eax; jl
    {
        // 0x00401918-0x00401925: push "Unable to allocate task type"; call Debug_Error; add $0x4,%esp; jmp $.
        Debug_Error("Unable to allocate task type");
        for (;;)
        {
        }
    }
    igi::retail::core::TaskTypeSlot& slot = ::QTask_TypeTable[type_id];
    // 0x0040193C-0x00401949: in_use byte, then object size, then the class name pointer.
    slot.in_use = 1U;                                        // 0x0040193C: movb $0x1,0xaf5f05(%esi)
    slot.object_size = object_size;                          // 0x00401943: mov %ecx,0xaf5ee4(%esi)
    slot.name = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(name));  // 0x00401949: mov %eax,0xaf5ee0(%esi)
    // 0x0040194F-0x0040195B: the parent id word, then the 0x0181 test that selects the inherited flags.
    slot.parent_type = parent_type;                          // 0x00401954: mov %di,0xaf5ee8(%esi)
    // 0x0040195D-0x00401976: parent slot +0x2C, indexed by the parent word with its low 16 bits only
    // (`and $0xffff,%eax` at 0x0040195F) and without a bounds check. The 0x0181 case is 0x00401978.
    if (parent_type != 0x0181U)                              // 0x0040194F: cmp $0x181,%di; je 0x00401978
    {
        slot.inherited_flags = ::QTask_TypeTable[parent_type].inherited_flags;
    }
    else
    {
        slot.inherited_flags = 0x20000000U;                  // 0x00401978: movl $0x20000000,0xaf5f0c(%esi)
    }
    // 0x00401982-0x004019A4: the fourth stack slot is the task-list argument. Non-zero builds the class list
    // through 0x004B0B00 at slot+0x0C with (object_size, count, 0) and sets +0x24; zero stores +0x24=0.
    if (task_list_count != 0U)                               // 0x00401986: test %eax,%eax; je 0x004019A6
    {
        // 0x0040198A-0x0040199A: push $0x0; push count; push object_size; push &slot+0x0C; call 0x004B0B00.
        (void)Mem_AllocIndexedBlock(slot.task_list, static_cast<std::int32_t>(object_size),
                                    static_cast<std::int32_t>(task_list_count), 0);
        slot.has_task_list = 1U;                             // 0x0040199D: movb $0x1,0xaf5f04(%esi)
    }
    else
    {
        slot.has_task_list = 0U;                             // 0x004019A6: movb $0x0,0xaf5f04(%esi)
    }
    // 0x004019AD-0x004019DD: copy branch, taken when the parent id is not 0x0181. ecx walks the new type's row
    // and eax the parent's row from 0x00A96AE0, both advanced 0x600 bytes per step, 0xFE entries; the arm ends
    // with its own 0x004014B0 call and `mov %bp,%ax` return.
    if (parent_type != 0x0181U)                              // 0x004019AD: cmp $0x181,%di; je 0x004019EB
    {
        for (std::uint32_t phase = 0U; phase < 0xFEU; ++phase)
        {
            ::QTask_HandlerTable[phase * igi::retail::core::task_phase_stride + type_id] =
                ::QTask_HandlerTable[phase * igi::retail::core::task_phase_stride + parent_type];
        }
        TaskType_RefreshHandlerMask();                       // 0x004019DF: call 0x004014B0
        return static_cast<std::uint16_t>(type_id);          // 0x004019E5: mov %bp,%ax
    }
    // 0x004019EB-0x00401A03: zero branch for the 0x0181 root marker, same 0x600-byte stride and 0xFE count.
    for (std::uint32_t phase = 0U; phase < 0xFEU; ++phase)
    {
        ::QTask_HandlerTable[phase * igi::retail::core::task_phase_stride + type_id] = nullptr;
    }
    TaskType_RefreshHandlerMask();                           // 0x00401A05: call 0x004014B0
    return static_cast<std::uint16_t>(type_id);              // 0x00401A0B: mov %bp,%ax
}

} // extern "C"

#undef NATIVE_DECL
