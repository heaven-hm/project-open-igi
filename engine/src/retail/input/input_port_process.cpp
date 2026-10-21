#include "igi/retail/input/input_port_process.hpp"

#include "igi/retail/core/task_types.hpp"
#include "igi/retail/mem/mem_alloc.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::input {
namespace {

// The port's record registry. The retail's list links hold the records'
// addresses; the port holds their indices so the record keeps the retail's
// 32-bit field offsets.
InputPortRecord* g_records[input_port_registry_size] = {};

} // namespace

InputPortRecord* InputPortRecordAt(std::uint32_t index) noexcept
{
    if (index >= input_port_registry_size)
    {
        return nullptr;
    }
    return g_records[index];
}

std::uint32_t RegisterInputPortRecord(InputPortRecord* record) noexcept
{
    for (std::uint32_t index = 0; index < input_port_registry_size; ++index)
    {
        if (g_records[index] == nullptr)
        {
            g_records[index] = record;
            return index;
        }
    }
    return input_port_registry_size;
}

} // namespace igi::retail::input

extern "C" {

std::uint16_t DAT_00A5EFA0 = 0U;
std::uint32_t DAT_00567C58 = 0xFFFFFFFFU;

// Native: InputPort_Process; retail VA 0x004ED5C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004ED5C0..0x004ED5D7.
// Behavior/CFG: reads the 16-bit input port index at 0x00A5EFA0, pushes the
//           object, the port index and zero, and calls the handler native at
//           0x004012A0. It returns nothing.
// Status/limits: STRUCTURALLY_VERIFIED; the load, the three pushes and the
//           call are the retail's.
void IGI_RETAIL_CDECL InputPort_Process(void* input_port)
{
    const std::uint16_t index = DAT_00A5EFA0;
    QTask_RegisterInputPortHandler(static_cast<std::uint32_t>(
                                       reinterpret_cast<std::uintptr_t>(input_port)),
                                   index, 0U);
}

// Native: QTask_RegisterInputPortHandler; retail VA 0x004012A0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004012A0..0x004012E8.
// Behavior/CFG: allocates the port record through the native at 0x004012F0,
//           links it into the handler list at 0x004AF8F0 when the context is
//           not null, and then dispatches it: the record's dispatch index at
//           +0x1C selects the handler in the table at 0x00A96AE0, and the
//           handler is called with the record and the object. The record is
//           returned in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the calls, the index read and the
//           dispatch are the retail's.
std::uint32_t IGI_RETAIL_CDECL QTask_RegisterInputPortHandler(
    std::uint32_t object, std::uint32_t port_index, std::uint32_t context)
{
    auto* const record = QTask_AllocateInputPort(port_index, context);
    if (record == nullptr)
    {
        return 0U;
    }
    if (context != 0U)
    {
        auto* const list = reinterpret_cast<igi::retail::input::HeapList*>(context);
        igi::retail::input::InsertInputPortRecord(list, record);
    }
    const std::uint16_t dispatch = record->dispatch_index;
    const auto handler = DAT_00A96AE0[dispatch];
    if (handler != nullptr)
    {
        handler(record);
    }
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(record));
}

// Native: QTask_AllocateInputPort; retail VA 0x004012F0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004012F0..0x004013B0.
// Behavior/CFG: increments the port counter at 0x00567C58, reads the class's
//           task list from the task type slot, and appends the port index to
//           it when the list has room. Otherwise it allocates the port record
//           with the class's object size through the native at 0x004B0C60. The
//           record is then cleared, its port index, dispatch index and
//           appended flag are stored, and the list node is linked. The record
//           is returned in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the counter, the slot reads, the
//           allocation and the record writes are the retail's.
igi::retail::input::InputPortRecord* IGI_RETAIL_CDECL QTask_AllocateInputPort(
    std::uint32_t port_index, std::uint32_t context)
{
    ++DAT_00567C58;
    const std::uint32_t slot = port_index & 0xFFFFU;
    auto& type_slot = DAT_00AF5EE0[slot];
    std::uint8_t appended = 0U;
    if (type_slot.has_task_list != 0U)
    {
        appended = 1U;
    }
    auto* const record = static_cast<igi::retail::input::InputPortRecord*>(
        igi::retail::mem::Mem_Alloc(static_cast<std::int32_t>(type_slot.object_size), 4));
    if (record == nullptr)
    {
        return nullptr;
    }
    record->unknown_0x00 = 0U;
    record->unknown_0x04 = 0U;
    record->port_index = port_index;
    record->unknown_0x18 = 0U;
    record->dispatch_index = static_cast<std::uint16_t>(context);
    record->appended = appended;
    // The retail links the record's list node: the next slot holds the
    // address of the previous slot, the previous slot is cleared, and the node
    // holds the address of the next slot. The port keeps the offsets.
    record->list_next = static_cast<std::uint32_t>(
        offsetof(igi::retail::input::InputPortRecord, list_previous));
    record->list_previous = 0U;
    record->list_node = static_cast<std::uint32_t>(
        offsetof(igi::retail::input::InputPortRecord, list_next));
    igi::retail::input::RegisterInputPortRecord(record);
    return record;
}

// The retail's list insert at 0x004AF8F0, adapted to the port's 32-bit record
// links.
void InsertInputPortRecord(
    igi::retail::input::HeapList* list, igi::retail::input::InputPortRecord* record) noexcept
{
    const std::uint32_t last = list->last;
    auto* const last_record = igi::retail::input::InputPortRecordAt(last);
    if (last_record != nullptr)
    {
        last_record->list_next = static_cast<std::uint32_t>(
            offsetof(igi::retail::input::InputPortRecord, list_previous));
    }
    record->list_previous = last;
    record->list_next = static_cast<std::uint32_t>(
        offsetof(igi::retail::input::HeapList, first));
    list->last = igi::retail::input::RegisterInputPortRecord(record);
}

} // extern "C"

#undef IGI_RETAIL_CDECL
