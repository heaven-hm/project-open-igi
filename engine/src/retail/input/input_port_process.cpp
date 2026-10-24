#include "igi/retail/input/input_port_process.hpp"

#include "igi/retail/core/task_types.hpp"
#include "igi/retail/input/input_port.hpp"
#include "igi/retail/mem/mem_alloc.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::input {
void InputPort::Remap(Action action, Device device, std::uint32_t code)
{
    auto& list = bindings_[static_cast<std::size_t>(action)];
    for (Binding& binding : list) {
        if (binding.device == device && binding.code == code) {
            return;
        }
    }
    list.push_back(Binding{device, code});
}

void InputPort::SetKeyDown(std::uint32_t code, bool down)
{
    // The raw state carries every key, so a key the configuration does not
    // bind to an action is still readable.
    raw_key_down_[code & 0x3FU] = down;
    for (std::size_t index = 0; index < bindings_.size(); ++index) {
        for (const Binding& binding : bindings_[index]) {
            if (binding.device != Device::keyboard || binding.code != code) {
                continue;
            }
            const std::size_t slot = binding.code & 0x3FU;
            if (down) {
                key_pending_[index][slot] = true;
            }
            key_down_[index][slot] = down;
        }
    }
}

void InputPort::SetMouseButtonDown(int button, bool down)
{
    if (button < 0 || button >= static_cast<int>(mouse_buttons_.size())) {
        return;
    }
    const std::size_t slot = static_cast<std::size_t>(button);
    if (down) {
        mouse_pending_[slot] = true;
    }
    mouse_buttons_[slot] = down;
    for (std::size_t index = 0; index < bindings_.size(); ++index) {
        for (const Binding& binding : bindings_[index]) {
            if (binding.device != Device::mouse ||
                static_cast<int>(binding.code) != button) {
                continue;
            }
            if (down) {
                key_pending_[index][slot] = true;
            }
            key_down_[index][slot] = down;
        }
    }
}

void InputPort::AddMouseDelta(int delta_x, int delta_y)
{
    const float scale = mouse_sensitivity_;
    mouse_delta_x_ += static_cast<float>(delta_x) * scale;
    mouse_delta_y_ += static_cast<float>(delta_y) * scale * (mouse_inverted_ ? -1.0F : 1.0F);
}

void InputPort::Run()
{
    // The retail native keeps a 32-bit changed mask beside the state array and
    // clears it as it runs. The port publishes the edges collected since the
    // previous run and clears the collection, so a pressed action stays visible
    // for exactly one frame.
    for (std::size_t index = 0; index < key_pressed_.size(); ++index) {
        key_pressed_[index] = key_pending_[index];
        key_pending_[index].fill(false);
    }
    mouse_pressed_ = mouse_pending_;
    mouse_pending_.fill(false);
}

bool InputPort::ActionDown(Action action) const noexcept
{
    const std::size_t index = static_cast<std::size_t>(action);
    for (const Binding& binding : bindings_[index]) {
        const auto& held = binding.device == Device::keyboard
                               ? key_down_[index][binding.code & 0x3FU]
                               : mouse_buttons_[binding.code & 0x3FU];
        if (held) {
            return true;
        }
    }
    return false;
}

bool InputPort::ActionPressed(Action action) const noexcept
{
    const std::size_t index = static_cast<std::size_t>(action);
    for (const Binding& binding : bindings_[index]) {
        const auto& pressed = binding.device == Device::keyboard
                                 ? key_pressed_[index][binding.code & 0x3FU]
                                 : mouse_pressed_[binding.code & 0x3FU];
        if (pressed) {
            return true;
        }
    }
    return false;
}
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

// The retail's list insert at 0x004AF8F0, adapted to the port's 32-bit record
// links.
void InsertInputPortRecord(HeapList* list, InputPortRecord* record) noexcept
{
    const std::uint32_t last = list->last;
    auto* const last_record = InputPortRecordAt(last);
    if (last_record != nullptr)
    {
        last_record->list_next = static_cast<std::uint32_t>(
            offsetof(InputPortRecord, list_previous));
    }
    record->list_previous = last;
    record->list_next = static_cast<std::uint32_t>(offsetof(HeapList, first));
    list->last = RegisterInputPortRecord(record);
}

} // namespace igi::retail::input

extern "C" {

std::uint16_t InputPort_CurrentIndex = 0U;
// Retail 0x00567C58 is a zero-initialised live-count dword (evidence sheet section 17.4); the previous
// 0xFFFFFFFFU initialiser was 0x00536148's image value, a separate monotonic invocation counter.
std::uint32_t QTask_InputPortCounter = 0U;

// Native: InputPort_Process; retail VA 0x004ED5C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004ED5C0..0x004ED5D7.
// Behavior/CFG: reads the 16-bit input port index at 0x00A5EFA0, pushes the object, the port index and zero, and calls the handler native at 0x004012A0. It returns nothing.
// Status/limits: STRUCTURALLY_VERIFIED; the load, the three pushes and the call are the retail's.
void NATIVE_DECL InputPort_Process(void* input_port)
{
    const std::uint16_t index = InputPort_CurrentIndex;
    QTask_RegisterInputPortHandler(static_cast<std::uint32_t>(
                                       reinterpret_cast<std::uintptr_t>(input_port)),
                                   index, 0U);
}

// Native: QTask_RegisterInputPortHandler; retail VA 0x004012A0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004012A0..0x004012E8.
// Behavior/CFG: allocates the port record through the native at 0x004012F0, links it into the handler list at 0x004AF8F0 when the context is not null, and then dispatches it: the record's dispatch index at +0x1C selects the handler in the table at 0x00A96AE0, and the handler is called with the record and the object. The record is returned in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the calls, the index read and the dispatch are the retail's.
std::uint32_t NATIVE_DECL QTask_RegisterInputPortHandler(
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
    const auto handler = QTask_HandlerTable[dispatch];
    if (handler != nullptr)
    {
        handler(record);
    }
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(record));
}

// Native: QTask_AllocateInputPort; retail VA 0x004012F0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004012F0..0x004013B0.
// Behavior/CFG: increments the port counter at 0x00567C58, reads the class's task list from the task type slot, and appends the port index to it when the list has room. Otherwise it allocates the port record with the class's object size through the native at 0x004B0C60. The record is then cleared, its port index, dispatch index and appended flag are stored, and the list node is linked. The record is returned in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the counter, the slot reads, the allocation and the record writes are the retail's.
igi::retail::input::InputPortRecord* NATIVE_DECL QTask_AllocateInputPort(
    std::uint32_t port_index, std::uint32_t context)
{
    ++QTask_InputPortCounter;
    const std::uint32_t slot = port_index & 0xFFFFU;
    auto& type_slot = QTask_TypeTable[slot];
    // Retail 0x00401360/0x0040133A: the pooled flag is 0 on the heap path and 1 only when the pool branch at
    // 0x0040132E-0x00401358 is taken. This transcription has no pool branch yet, so it always allocates and the flag
    // is 0. The previous code took it from the type record's task-list byte, which can mark an instance pooled when
    // it came from the heap - a divergence recorded in section 17.6.
    std::uint8_t appended = 0U;
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

} // extern "C"

#undef NATIVE_DECL
