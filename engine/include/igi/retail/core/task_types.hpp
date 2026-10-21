#pragma once

// The retail task type registry and the handler table.
//
// igi.exe registers every task class at start-up (the 0x402930 sequence) and
// links the class handlers into one table. The per-frame dispatch at 0x00401050
// reads that table, so the engine follows the retail flow only when the
// registry and the table hold the same entries the retail fills.

#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::core {

// The retail task type slot, 0x30 bytes, as the retail lays it out. The name
// is a pointer in the retail and a pointer in the port, so the slot is wider
// than the retail's on a 64-bit build; every field before it keeps the retail
// offset, and the name is the last field so nothing after it shifts.
struct TaskTypeSlot
{
    std::uint32_t name_index;                // +0x00, 0x00AF5EE0: the retail
                                              // stores the name pointer here, and
                                              // the retail is 32-bit, so the
                                              // port keeps the slot layout by
                                              // holding the name by index.
    std::uint32_t object_size;               // +0x04, 0x00AF5EE4
    std::uint16_t parent_type;               // +0x08, 0x00AF5EE8
    std::uint8_t unknown_0x0A[0x1A];
    std::uint8_t has_task_list;              // +0x24, 0x00AF5F04
    std::uint8_t in_use;                     // +0x25, 0x00AF5F05
    std::uint8_t unknown_0x26[0x06];
    std::uint32_t inherited_flags;           // +0x2C, 0x00AF5F0C
};
static_assert(offsetof(TaskTypeSlot, object_size) == 0x04, "the retail slot layout");
static_assert(offsetof(TaskTypeSlot, parent_type) == 0x08, "the retail slot layout");
static_assert(offsetof(TaskTypeSlot, in_use) == 0x25, "the retail slot layout");
static_assert(offsetof(TaskTypeSlot, inherited_flags) == 0x2C, "the retail slot layout");

// The class handler the retail stores in the handler table.
using FrameHandler = void (*)(void*);

// The number of task type slots the retail reserves: 0x00AF5F05 to 0x00AFA705
// in 0x30-byte steps.
inline constexpr std::size_t task_type_slot_count = 0x4800U / 0x30U;

// The handler table index stride per class: 384 phases. The retail computes
// the index as class_id + phase * 384 at 0x00401400.
inline constexpr std::uint32_t task_phase_stride = 0x180U;

// The retail event table at 0x0054F930. Each entry is six bytes, and the
// retail allocates the event identifiers from 0x20.
inline constexpr std::size_t event_table_entry_count = 0x540U / 6U;
inline constexpr std::uint8_t first_event_id = 0x20U;

} // namespace igi::retail::core

extern "C" {

// The retail task type table at 0x00AF5EE0. The retail scans the in-use byte
// at 0x00AF5F05, which is slot + 0x25.
extern igi::retail::core::TaskTypeSlot DAT_00AF5EE0[igi::retail::core::task_type_slot_count];

// The retail handler table at 0x00A96AE0. The entry for a class is at
// class_id * 0x180 + phase, so the phase 1 table starts at 0x00A970E0, which is
// the table the per-frame dispatch at 0x00401050 reads. The retail stores the
// class handler itself in the table and the per-frame calls it.
extern igi::retail::core::FrameHandler DAT_00A96AE0[];

// The retail's event table at 0x0054F930. Each entry is six bytes.
extern std::uint8_t DAT_0054F930[igi::retail::core::event_table_entry_count * 6U];

// The retail's current task type, a word at 0x0054F928.
extern std::uint16_t DAT_0054F928;

// The retail's first-link flag table at 0x0054FF34.
extern std::uint8_t DAT_0054FF34[igi::retail::core::task_type_slot_count *
                                  igi::retail::core::task_phase_stride];

// The retail's linked-handler counter at 0x00536148.
extern std::uint32_t DAT_00536148;

// The retail's context-handler count at 0x00567C6C and its no-context count
// at 0x00567C70.
extern std::uint32_t DAT_00567C6C;
extern std::uint32_t DAT_00567C70;

// The phase 1 base of the handler table, 0x00A970E0.
inline igi::retail::core::FrameHandler* DAT_00A970E0()
{
    return DAT_00A96AE0 + igi::retail::core::task_phase_stride;
}

// Native: TaskType_Register() - VA 0x00401900 - complete port.
// Behavior: scans the type table at 0x00AF5F05 for a free slot in 0x30-byte
//           steps, marks it used, stores the name at slot+0x25, the object size
//           at slot+0x21, the parent type at slot+0x1D, and takes the inherited
//           flags at slot+0x07 from the parent slot, except for the parent
//           0x0181 which carries 0x20000000. It returns the 16-bit type id.
std::uint16_t IGI_RETAIL_CDECL TaskType_Register(const char* name, std::uint32_t object_size,
                                                 std::uint16_t parent_type, std::uint32_t flags);

// Native: QTask_LinkQTaskEventHandlerNoContextFn() - VA 0x00401530 - complete port.
// Behavior: increments the no-context handler count at 0x00567C70 and the
//           linked-handler counter at 0x00536148, computes the handler table
//           index as class_id + phase * 384, stores the handler at
//           0x00A96AE0 + index * 4 and the first-link flag at 0x0054FF34 +
//           index.
std::uint32_t IGI_RETAIL_CDECL QTask_LinkQTaskEventHandlerNoContextFn(
    std::uint16_t event_id, std::uint32_t context,
    igi::retail::core::FrameHandler handler);

// Native: QTask_LinkQTaskEventHandler() - VA 0x00401400 - complete port.
// Behavior: the context variant of the link native. It increments the context
//           handler count at 0x00567C6C instead of 0x00567C70 and otherwise
//           stores the handler and the flag the same way.
std::uint32_t IGI_RETAIL_CDECL QTask_LinkQTaskEventHandler(
    std::uint16_t event_id, std::uint32_t context,
    igi::retail::core::FrameHandler handler);

// Native: QTask_GetCurrentType() - VA 0x004018F0 - complete port.
// Behavior: returns the 16-bit current task type at 0x0054F928 in AX.
std::uint16_t IGI_RETAIL_CDECL QTask_GetCurrentType();

// Native: QTask_AllocateEvent() - VA 0x004017C0 - complete port.
// Behavior: scans the event table from id 0x20 in six-byte steps for a free
//           entry, marks it used, stores the argument in the entry's first
//           word and returns the event id in AL. It raises "Unable to
//           allocate event" when the table is full.
std::uint8_t IGI_RETAIL_CDECL QTask_AllocateEvent(std::uint16_t value);

} // extern "C"

#undef IGI_RETAIL_CDECL
