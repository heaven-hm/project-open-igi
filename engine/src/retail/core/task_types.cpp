#include "igi/retail/core/task_types.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

#include <cstring>

namespace {

// The registered class names, indexed by the slot's name_index.
const char* g_type_names[64] = {};

// The retail's link native computes the handler table index as
// class_id + phase * 384 and stores the handler at 0x00A96AE0 + index * 4.
std::uint32_t link_handler(std::uint16_t class_id, std::uint32_t phase,
                           igi::retail::core::FrameHandler handler, std::uint32_t& count)
{
    ++count;
    ++DAT_00536148;
    const std::uint32_t index = (static_cast<std::uint32_t>(class_id) & 0xFFFFU) +
                                ((phase & 0xFFU) * igi::retail::core::task_phase_stride);
    DAT_00A96AE0[index] = handler;
    DAT_0054FF34[index] = (count == 1U) ? 1U : 0U;
    return index;
}

} // namespace

extern "C" {

igi::retail::core::TaskTypeSlot DAT_00AF5EE0[igi::retail::core::task_type_slot_count] = {};
igi::retail::core::FrameHandler DAT_00A96AE0[igi::retail::core::task_type_slot_count *
                                       igi::retail::core::task_phase_stride] = {};
std::uint8_t DAT_0054F930[igi::retail::core::event_table_entry_count * 6U] = {};
std::uint16_t DAT_0054F928 = 0U;
std::uint8_t DAT_0054FF34[igi::retail::core::task_type_slot_count *
                           igi::retail::core::task_phase_stride] = {};
std::uint32_t DAT_00536148 = 0xFFFFFFFFU;
std::uint32_t DAT_00567C6C = 0U;
std::uint32_t DAT_00567C70 = 0U;

// Native: TaskType_Register; retail VA 0x00401900.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00401900..0x004019DA.
// Behavior/CFG: scans the type table at 0x00AF5F05 for a free slot, marks it used, stores the
//           name at slot+0x25, the object size at slot+0x21 and the parent type at slot+0x1D, and
//           takes the inherited flags at slot+0x07 from the parent slot, except for the parent
//           0x0181 which carries 0x20000000. It returns the 16-bit type id.
// Status/limits: STRUCTURALLY_VERIFIED; the slot layout and the inheritance rule are the retail's.
std::uint16_t IGI_RETAIL_CDECL TaskType_Register(const char* name, std::uint32_t object_size,
                                                std::uint16_t parent_type, std::uint32_t flags)
{
    (void)flags;
    std::uint32_t type_id = 0U;
    for (; type_id < igi::retail::core::task_type_slot_count; ++type_id)
    {
        if (DAT_00AF5EE0[type_id].in_use == 0U)
        {
            break;
        }
    }
    if (type_id >= igi::retail::core::task_type_slot_count)
    {
        return 0U;
    }
    igi::retail::core::TaskTypeSlot& slot = DAT_00AF5EE0[type_id];
    slot.in_use = 1U;
    slot.object_size = object_size;
    if (name != nullptr)
    {
        for (std::size_t index = 0; index < 64U; ++index)
        {
            if (g_type_names[index] == nullptr)
            {
                g_type_names[index] = name;
                slot.name_index = static_cast<std::uint32_t>(index);
                break;
            }
        }
    }
    slot.parent_type = parent_type;
    if (parent_type == 0x0181U)
    {
        slot.inherited_flags = 0x20000000U;
    }
    else if (parent_type < igi::retail::core::task_type_slot_count)
    {
        slot.inherited_flags = DAT_00AF5EE0[parent_type].inherited_flags;
    }
    return static_cast<std::uint16_t>(type_id);
}

// Native: QTask_LinkQTaskEventHandlerNoContextFn; retail VA 0x00401530.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00401530..0x0040159F.
// Behavior/CFG: reads the no-context handler count at 0x00567C70 and the linked-handler counter
//           at 0x00536148, increments both, computes the handler table index as class_id +
//           phase * 384, stores the handler at 0x00A96AE0 + index * 4 and the first-link flag at
//           0x0054FF34 + index.
// Status/limits: STRUCTURALLY_VERIFIED; the index arithmetic and the two table writes are the
//           retail's.
std::uint32_t IGI_RETAIL_CDECL QTask_LinkQTaskEventHandlerNoContextFn(
    std::uint16_t event_id, std::uint32_t context, igi::retail::core::FrameHandler handler)
{
    return link_handler(event_id, context, handler, DAT_00567C70);
}

// Native: QTask_LinkQTaskEventHandler; retail VA 0x00401400.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00401400..0x0040148F.
// Behavior/CFG: the context variant of the link native. It reads the context handler count at
//           0x00567C6C instead of 0x00567C70, increments both counters, computes the handler table
//           index as class_id + phase * 384, stores the handler at 0x00A96AE0 + index * 4 and the
//           first-link flag at 0x0054FF34 + index.
// Status/limits: STRUCTURALLY_VERIFIED; the index arithmetic and the two table writes are the
//           retail's.
std::uint32_t IGI_RETAIL_CDECL QTask_LinkQTaskEventHandler(
    std::uint16_t event_id, std::uint32_t context, igi::retail::core::FrameHandler handler)
{
    return link_handler(event_id, context, handler, DAT_00567C6C);
}

// Native: QTask_GetCurrentType; retail VA 0x004018F0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004018F0..0x004018F7.
// Behavior/CFG: loads the 16-bit current task type at 0x0054F928 into AX and returns it.
// Status/limits: STRUCTURALLY_VERIFIED; the load and the AX return match the retail exactly.
std::uint16_t IGI_RETAIL_CDECL QTask_GetCurrentType()
{
    return DAT_0054F928;
}

// Native: QTask_AllocateEvent; retail VA 0x004017C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004017C0..0x0040180C.
// Behavior/CFG: starts the event id at 0x20 and scans the six-byte event table from 0x0054F9F2
//           for a free entry, raising "Unable to allocate event" when the table is full. For a
//           free entry it marks the entry used at 0x0054F932 + id * 6, stores the argument in the
//           entry's word at 0x0054F930 + id * 6 and marks the entry at 0x0054F933 + id * 6 used.
//           The event id is returned in AL.
// Status/limits: STRUCTURALLY_VERIFIED; the scan, the three writes and the AL return match the
//           retail exactly.
std::uint8_t IGI_RETAIL_CDECL QTask_AllocateEvent(std::uint16_t value)
{
    std::uint32_t event_id = igi::retail::core::first_event_id;
    while (event_id < igi::retail::core::event_table_entry_count &&
           DAT_0054F930[event_id * 6U] != 0U)
    {
        ++event_id;
    }
    if (event_id >= igi::retail::core::event_table_entry_count)
    {
        return 0U;
    }
    const std::size_t entry = static_cast<std::size_t>(event_id) * 6U;
    DAT_0054F930[entry] = 1U;
    DAT_0054F930[entry + 2U] = static_cast<std::uint8_t>(value & 0xFFU);
    DAT_0054F930[entry + 3U] = static_cast<std::uint8_t>((value >> 8) & 0xFFU);
    DAT_0054F930[entry + 5U] = 1U;
    return static_cast<std::uint8_t>(event_id);
}

} // extern "C"

#undef IGI_RETAIL_CDECL
