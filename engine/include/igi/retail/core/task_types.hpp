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
//
// KNOWN-POSSIBLY-UNDER-SPECIFIED. This type is a placeholder for a table whose
// slots do not share one signature, and it is deliberately NOT widened. Do not
// treat the single parameter as a claim about what retail passes; a cast to this
// type is lossy and says nothing about the callee's real argument count.
//
// Measured by scripts/handler_abi_census.py over the 169 handler addresses the
// tree actually installs, intersecting them with the image bodies and simulating
// esp and any ebp frame (entry esp = 0; [esp+8] = arg2; [ebp+0xC] = arg2):
//   - 86 read argument 1 only
//   - 82 read argument 2 (49%): 67 in other shapes, 15 in the 4-instruction
//     `mov eax,[esp+8]; mov ecx,[global]; mov [eax],ecx; ret` shape
//   - 1 reads neither
//   - controls: QTask_NoOpHandler 0x004D3710 reads neither;
//     Script_SetConstant_51BAA0 reads arg2
//
// The invocation sites of the table 0x00401400/0x00401530 write
// ([ecx*4+0xA96AE0]) agree with two arguments: over its 28 call sites, 19 push two
// consecutive arguments, 5 push one, 4 push none immediately. See 0x004D1324
// (`push edx; push eax; call [ecx*4+0xA96AE0]`) and 0x004E6C4F.
//
// COUNTER-EVIDENCE, UNRESOLVED: the two walk-and-dispatch routines 0x00424280
// (`push esi; call eax; add esp,4`) and 0x00488700 (`push ecx; call eax; add esp,4`)
// read the same table and push exactly one argument. Retail therefore invokes some
// slots with one argument and others with two, and no transcribed body reconciles
// that. The ABI appears to be per-slot, not per-table.
//
// What a caller must do: pass BOTH arguments when transcribing the dispatcher,
// rather than re-deriving the count from this type. The 15 set-constant handlers
// each write one of the band's container globals, a measured 1:1 pairing
// (e.g. 0x00429890 -> 0x0057BC94, the handler registrar 0x00427FF0 links in its
// event 4, writing that registrar's own container global).
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
// Transcribed at engine/src/retail/core/native_401900.cpp, including the
//           0x004B0B00 task-list creation at slot+0x0C and both tail loops over
//           the parent's 0xFE handler phases (copy at 0x004019B4-0x004019DD,
//           zero for parent 0x0181 at 0x004019EB-0x00401A03).
std::uint16_t NATIVE_DECL TaskType_Register(const char* name, std::uint32_t object_size,
                                                 std::uint16_t parent_type,
                                                 std::uint32_t task_list_argument);
// Call-site arity differs from callee arity, established from 0x00401900's own bytes:
// every retail registration call site pushes FIVE dwords (the 5-push / add esp,0x38
// pattern), but the callee reads only four -- [esp+0xC], [esp+8], then after saving two
// registers [esp+0x18] and [esp+0x1C]. Nothing in the body reads the fifth slot. The
// extra dword is pushed and cleaned by every call site and ignored by the callee, so the
// port's four-parameter signature is correct and the generated code legitimately emits
// four pushes. This is why a provenance block counting "five registration pushes" is
// describing retail's stack, not this signature.

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
