#pragma once

// The retail input port registry and the input port natives.
//
// igi.exe runs one input port per device and processes them from the game's
// per-frame phase. The port index at 0x00A5EFA0 selects the port the native
// processes, the port record is a task object of its class, and the port's
// handler is looked up in the same handler table at 0x00A96AE0 the per-frame
// dispatch uses.
//
// The retail is a 32-bit build, so the record's links are pointers. The port
// keeps the retail's field offsets and holds the links as offsets inside the
// record, with the list's last node held as an index into the port's record
// registry, the way the task type slot holds the class name by index.

#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::input {

// The retail input port record, as the native at 0x004012F0 lays it out.
// The record is the task object of the port's class, so the class's object
// size decides the allocation.
struct InputPortRecord
{
    std::uint32_t unknown_0x00;      // +0x00, cleared by the native
    std::uint32_t unknown_0x04;      // +0x04, cleared by the native
    std::uint32_t list_next;         // +0x08, the list node's next
    std::uint32_t list_previous;     // +0x0C, the list node's previous
    std::uint32_t list_node;         // +0x10, the list node
    std::uint32_t port_index;        // +0x14, the port index
    std::uint32_t unknown_0x18;      // +0x18, cleared by the native
    std::uint16_t dispatch_index;    // +0x1C, the handler table index
    std::uint8_t appended;           // +0x1E, set when the port joined a task list
    std::uint8_t unknown_0x1F;
};

// The retail heap list, as the native at 0x004AF8F0 uses it. The retail's
// list holds the last node's address; the port holds the record's index in
// the record registry.
struct HeapList
{
    std::uint32_t unknown_0x00;      // +0x00
    std::uint32_t first;             // +0x04, the list's first node
    std::uint32_t last;              // +0x08, the list's last node
};

// The port's input port record registry. The retail's list links hold the
// records' addresses; the port holds their indices so the record keeps the
// retail's 32-bit field offsets.
inline constexpr std::size_t input_port_registry_size = 64U;

InputPortRecord* InputPortRecordAt(std::uint32_t index) noexcept;
std::uint32_t RegisterInputPortRecord(InputPortRecord* record) noexcept;

// The retail's list insert at 0x004AF8F0, adapted to the port's 32-bit record
// links: the record's previous becomes the list's last, the record's next
// becomes the list's first slot, the last's next becomes the record and the
// list's last becomes the record. The retail's own HeapList_Insert native
// stays in the memory module for the allocator's free list.
void InsertInputPortRecord(HeapList* list, InputPortRecord* record) noexcept;

} // namespace igi::retail::input

extern "C" {

// The retail input port index at 0x00A5EFA0. The native reads it to select
// the port it processes.
extern std::uint16_t DAT_00A5EFA0;

// The retail input port counter at 0x00567C58.
extern std::uint32_t DAT_00567C58;

// Native: InputPort_Process() - VA 0x004ED5C0 - complete port.
// Behavior: reads the 16-bit input port index at 0x00A5EFA0, and calls the
//           handler native at 0x004012A0 with the object, the port index and
//           zero.
void IGI_RETAIL_CDECL InputPort_Process(void* input_port);

// Native: QTask_RegisterInputPortHandler() - VA 0x004012A0 - complete port.
// Behavior: allocates the port record through the native at 0x004012F0, links
//           it into the handler list at 0x004AF8F0 when the context is not
//           null, and then dispatches it: the record's dispatch index at
//           +0x1C selects the handler in the table at 0x00A96AE0, and the
//           handler is called with the record and the object.
std::uint32_t IGI_RETAIL_CDECL QTask_RegisterInputPortHandler(
    std::uint32_t object, std::uint32_t port_index, std::uint32_t context);

// Native: QTask_AllocateInputPort() - VA 0x004012F0 - complete port.
// Behavior: reads the class's task list from the task type slot, appends the
//           port index to it when the list has room, and otherwise allocates
//           the port record with the class's object size through the native at
//           0x004B0C60. The record is cleared, its port index, dispatch index
//           and appended flag are stored, and the list node is linked.
igi::retail::input::InputPortRecord* IGI_RETAIL_CDECL QTask_AllocateInputPort(
    std::uint32_t port_index, std::uint32_t context);

} // extern "C"

#undef IGI_RETAIL_CDECL
