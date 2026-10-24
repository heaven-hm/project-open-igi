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
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::core {

// The retail task type slot, 0x30 bytes, as the retail lays it out. The name
// is a pointer in the retail and a pointer in the port, so the slot is wider
// than the retail's on a 64-bit build; every field before it keeps the retail
// offset, and the name is the last field so nothing after it shifts.
struct TaskTypeSlot
{
    std::uint32_t name;                      // +0x00, 0x00AF5EE0: the class-name pointer (32-bit target only)
    std::uint32_t object_size;               // +0x04, 0x00AF5EE4
    std::uint16_t parent_type;               // +0x08, 0x00AF5EE8
    std::uint8_t unknown_0x0A[0x02];
    std::int32_t task_list[6];               // +0x0C..+0x23, written by 0x004B0B00
    std::uint8_t has_task_list;              // +0x24, 0x00AF5F04
    std::uint8_t in_use;                     // +0x25, 0x00AF5F05
    std::uint8_t unknown_0x26[0x02];
    std::uint16_t handler_mask;              // +0x28, written by 0x004014B0
    std::uint8_t unknown_0x2A[0x02];
    std::uint32_t inherited_flags;           // +0x2C, 0x00AF5F0C
};
static_assert(offsetof(TaskTypeSlot, handler_mask) == 0x28, "the retail slot layout");
static_assert(sizeof(TaskTypeSlot) == 0x30, "the retail slot is 0x30 bytes");
static_assert(offsetof(TaskTypeSlot, object_size) == 0x04, "the retail slot layout");
static_assert(offsetof(TaskTypeSlot, parent_type) == 0x08, "the retail slot layout");
static_assert(offsetof(TaskTypeSlot, task_list) == 0x0C, "the retail slot layout");
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
// Phase count of the handler/state tables, from retail 0x004019C1 (`mov edx,0xFE`) and the initialiser's
// zero-fill count at 0x0040117D (`mov ecx,0x17d00` = 0xFE * 0x180). The stride is the class count; these are
// two different dimensions and must not be used interchangeably for sizing.
inline constexpr std::size_t task_phase_count = 0xFEU;

// The retail event table at 0x0054F930. Each entry is six bytes, and the
// retail allocates the event identifiers from 0x20.
// Retail 0x0054F930..0x0054FF24 is the phase-record table: 0x5F4 bytes of 6-byte records = 254 records, proven by
// the clear loop at 0x004011D5 (`mov eax,0x54F932 / add eax,6 / cmp eax,0x54FF26 / jl`), not 0x540/6 = 224 from the
// earlier count. The clear loop and both allocator scan bounds were 30 records short.
inline constexpr std::size_t event_table_entry_count = 0x5F4U / 6U;
inline constexpr std::uint8_t first_event_id = 0x20U;

} // namespace igi::retail::core

extern "C" {

// Native: TaskType_RefreshHandlerMask() - VA 0x004014B0.
// Behavior: for every in-use type slot, stores the 16-bit per-phase handler mask
// from 0x004014E0 at slot+0x28.
void TaskType_RefreshHandlerMask();

// The retail task type table at 0x00AF5EE0. The retail scans the in-use byte
// at 0x00AF5F05, which is slot + 0x25.
extern igi::retail::core::TaskTypeSlot QTask_TypeTable[igi::retail::core::task_type_slot_count];

// The retail handler table at 0x00A96AE0. The entry for a class is at
// class_id * 0x180 + phase, so the phase 1 table starts at 0x00A970E0, which is
// the table the per-frame dispatch at 0x00401050 reads. The retail stores the
// class handler itself in the table and the per-frame calls it.
extern igi::retail::core::FrameHandler QTask_HandlerTable[];

// Retail global at 0x0054FF24: the 16-byte phase map read by 0x004014E0 (0xFF =
// phase absent). The retail symbol is unknown; address-named until evidence names
// it. Written by the 0x00401400/0x00401530 link natives, not by 0x004014E0/B0.
extern std::uint8_t g_54ff24[16];

// The retail's event table at 0x0054F930. Each entry is six bytes: the
// argument word at +0x00, the used byte at +0x02 and the second used byte at
// +0x03.
extern std::uint8_t QTask_EventTable[igi::retail::core::event_table_entry_count * 6U];

// The retail's current task type, a word at 0x0054F928.
extern std::uint16_t QTask_CurrentType;

// The retail's first-link flag table at 0x0054FF34.
extern std::uint8_t QTask_FirstLinkFlag[igi::retail::core::task_type_slot_count *
                                  igi::retail::core::task_phase_count];

// The retail's linked-handler counter at 0x00536148.
extern std::uint32_t QTask_LinkedHandlerCounter;

// The retail's context-handler count at 0x00567C6C and its no-context count
// at 0x00567C70.
extern std::uint32_t QTask_ContextHandlerCount;
extern std::uint32_t QTask_NoContextHandlerCount;

// The phase 1 base of the handler table, 0x00A970E0.
inline igi::retail::core::FrameHandler* QTask_Phase1HandlerTable()
{
    return QTask_HandlerTable + igi::retail::core::task_phase_stride;
}

// Native: TaskType_Register() - VA 0x00401900.
// Behavior: scans the type table from 0x00AF5EE0 for a free 0x30-byte slot,
//           marks it used at slot+0x25, stores the name pointer at slot+0x00,
//           the object size at slot+0x04 and the parent type's low 16 bits at
//           slot+0x08, and takes the inherited flags at slot+0x2C from the
//           parent slot, except for the parent 0x0181 which carries
//           0x20000000. The fourth stack slot is the task-list argument: when
//           it is non-zero the retail builds the class task list through
//           0x004B0B00 at slot+0x0C and sets the has_task_list byte at
//           slot+0x24. It returns the 16-bit type id.
// Limits: the port records the task-list argument but does not transcribe the
//           0x004B0B00 creation path, and it does not copy the parent's
//           handler phases (the retail tail loop at 0x004019CD); see
//           task_types.cpp for the full evidence record.
std::uint16_t NATIVE_DECL TaskType_Register(const char* name, std::uint32_t object_size,
                                                 std::uint16_t parent_type,
                                                 std::uint32_t task_list_argument);

// Port-side startup replay bound to the live 0x00401900 registration capture
// (docs/fidelity/evidence/0x00401900-retail-registration-order.json); not a
// retail native. Registers every recorded class whose id falls in the
// inclusive range and fails on an id mismatch, because the per-frame dispatch
// indexes the handler table by class id and one skipped registration shifts
// every later id. Ids 0 (QTask), 124 (Game) and 125 (Flow) stay with
// QTask_Init, Game_Init and Flow_Init; the startup replays 1..123 before
// Game_Init and 126..278 after Flow_Init.
void TaskTypes_RegisterRetailClasses(std::uint16_t first_id, std::uint16_t last_id);

// Native: QTask_LinkQTaskEventHandlerNoContextFn() - VA 0x00401530 - complete port.
// Behavior: increments the no-context handler count at 0x00567C70 and the
//           linked-handler counter at 0x00536148, computes the handler table
//           index as class_id + event_id * 384 (no mask: the full 32-bit event
//           word is the phase), stores the handler at
//           0x00A96AE0 + index * 4 and the first-link flag at 0x0054FF34 +
//           index.
std::uint32_t NATIVE_DECL QTask_LinkQTaskEventHandlerNoContextFn(
    std::uint16_t class_id, std::uint32_t event_id,
    igi::retail::core::FrameHandler handler);

// Native: QTask_LinkQTaskEventHandler() - VA 0x00401400 - complete port.
// Behavior: the context variant of the link native. It increments the context
//           handler count at 0x00567C6C instead of 0x00567C70, masks the event
//           word to its low byte for the phase (index = class_id + (event_id &
//           0xFF) * 384) and stores the handler and the flag the same way.
std::uint32_t NATIVE_DECL QTask_LinkQTaskEventHandler(
    std::uint16_t class_id, std::uint32_t event_id,
    igi::retail::core::FrameHandler handler);

// Native: QTask_GetCurrentType() - VA 0x004018F0 - complete port.
// Behavior: returns the 16-bit current task type at 0x0054F928 in AX.
// noinline is a compiler code-generation directive, no added logic: retail QTask_Init calls this native four
// times (0x00401140, 0x00401150, 0x00401160, 0x00401170) and those call edges are part of the CFG this project
// scores. Letting -O3 inline the body would delete four retail edges while every value stayed correct.
#if defined(__GNUC__) || defined(__clang__)
__attribute__((noinline))
#endif
std::uint16_t NATIVE_DECL QTask_GetCurrentType();

// Native: QTask_Init() - VA 0x004010D0 - complete port.
// Behavior: zeroes the 0x40 dwords at 0x00AFA6E0, clears the word at
//           0x00AFA7E0 and the dword at 0x0054F92C, clears the event table
//           through 0x004011D0 and the task type table through 0x004011B0,
//           fills the 0x3F dwords at 0x00A969E0 with -1, registers the
//           "QTask" class with a 0x20 byte object and the parent 0x0181, keeps
//           the class id at 0x0054F928, and links the phase handlers.
void NATIVE_DECL QTask_Init();

// Native: QTask_ClearEventTable() - VA 0x004011D0 - complete port.
// Behavior: clears the used byte of every six-byte event entry from
//           0x0054F932 to 0x0054FF26.
void NATIVE_DECL QTask_ClearEventTable();

// Native: QTask_ClearTaskTypes() - VA 0x004011B0 - complete port.
// Behavior: clears the in-use byte at slot + 0x25 and the name dword at slot
//           + 0x00 for every 0x30-byte task type slot from 0x00AF5EE0 to
//           0x00AFA6E0.
void NATIVE_DECL QTask_ClearTaskTypes();

// Native: QTask_AllocateEvent() - VA 0x004017C0 - complete port.
// Behavior: scans the event table from id 0x20 in six-byte steps for a free
//           entry, marks it used, stores the argument in the entry's first
//           word and returns the event id in AL. It raises "Unable to
//           allocate event" when the table is full.
std::uint32_t NATIVE_DECL QTask_AllocateEvent(std::uint16_t value);  // retail returns the full id in EAX (AL read by callers)

} // extern "C"

#undef NATIVE_DECL
