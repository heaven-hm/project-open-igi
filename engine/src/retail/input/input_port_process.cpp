#include "dev_log.hpp"
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

// Retail natives 0x004B0D60 and 0x004B0D90, defined in engine/src/retail/mem. Declared here exactly as retail
// declares them: the allocator calls the pair around Mem_Alloc.
void Mem_FlagStackPop(std::uint8_t const flag);
void Mem_FlagStackPush();

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
    NATIVE_LOG_ENTER("InputPort_Process", "~0x004ED5C0");
    const std::uint16_t index = InputPort_CurrentIndex;
    QTask_RegisterInputPortHandler(static_cast<std::uint32_t>(
                                       reinterpret_cast<std::uintptr_t>(input_port)),
                                   index, 0U);
}

// Native: QTask_RegisterInputPortHandler; retail VA 0x004012A0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004012A0..0x004012E7; evidence sheet section 17.
// Behavior/CFG: calls the allocator 0x004012F0 with (arg1, arg2), links the new task into the list at arg1+8 through 0x004AF8F0 when arg1 is not null, then reads the handler index from the object's own +0x1C word, dispatches QTask_HandlerTable[index] with two cdecl arguments (the object, then arg3) and cleans 8 bytes; returns the object.
// Status/limits: TRANSCRIBED; the argument order, the arg1 null test, the +0x1C read and the two-argument dispatch follow section 17. Retail handlers are bare function pointers with no single signature: this site pushes two words and cleans 8 (0x004012DC-0x004012E0) while the Flow walks at 0x00402365, 0x004023CE and 0x00402481 push one word and clean 4, so the cast at this call site is the faithful representation and not a temporary workaround.
std::uint32_t NATIVE_DECL QTask_RegisterInputPortHandler(
    std::uint32_t object, std::uint32_t port_index, std::uint32_t context)
{
    NATIVE_LOG_ENTER("QTask_RegisterInputPortHandler", "~0x004ED5C0");
    std::uint8_t* const owner = reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(object)); // 0x004012A6
    auto* const record = QTask_AllocateInputPort(object, port_index);                                  // 0x004012AC
    std::uint8_t* const task = reinterpret_cast<std::uint8_t*>(record);
    if (owner != nullptr)                                                                              // 0x004012B6
    {
        igi::retail::input::InsertInputPortRecord(                                                     // 0x004012BF -> 0x004AF8F0
            reinterpret_cast<igi::retail::input::HeapList*>(owner + 8), record);
    }
    const std::uint16_t index = *reinterpret_cast<std::uint16_t*>(task + 0x1C);                        // 0x004012C9
    const igi::retail::core::FrameHandler handler = ::QTask_HandlerTable[index];      // 0x004012CD
    if (handler != nullptr)                                                                            // 0x004012D6
    {
        using PhaseZeroHandler = void(NATIVE_DECL*)(void*, std::uint32_t);
        reinterpret_cast<PhaseZeroHandler>(handler)(task, context);                                     // 0x004012DC-0x004012E0
    }
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(task));                         // 0x004012E3
}

// Native: QTask_AllocateInputPort; retail VA 0x004012F0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004012F0..0x004013AC; evidence sheet section 17.
// Behavior/CFG: increments the live task count at 0x00567C58 unconditionally before any branch, then either takes a slot from the type's pool (+0x24 flag, +0x14 used, +0x18 capacity, +0x10 slot array, +0x1C element size, +0x0C base) or pops the memory flag stack with 1, allocates the type's +0x04 object size with alignment 4 and pushes the flag stack back. It then writes the header in the retail order +4, +0, +0x14, +0x1E, +0x1C, +0x18, +8, +0xC, +0x10 and returns the object.
// Status/limits: TRANSCRIBED; the pool branch, the flag-stack pair, the unconditional and pre-branch counter increment and the store order are transcribed, but the store at 0x0040134E is reproduced literally and its meaning is unsettled in section 17.7.
igi::retail::input::InputPortRecord* NATIVE_DECL QTask_AllocateInputPort(
    std::uint32_t owner, std::uint32_t type_id)
{
    NATIVE_LOG_ENTER("QTask_AllocateInputPort", "~0x004ED5C0");
    ++QTask_InputPortCounter;                                                                          // 0x004012F9/0x00401307/0x0040130B
    std::uint8_t* const type_record = reinterpret_cast<std::uint8_t*>(
        &::QTask_TypeTable[static_cast<std::size_t>(type_id) & 0xFFFFU]);
    std::uint8_t pooled = 0U;
    std::uint8_t* task = nullptr;
    if (type_record[0x24] != 0U &&                                                                     // 0x00401314/0x0040131A
        *reinterpret_cast<std::uint32_t*>(type_record + 0x14) <
            *reinterpret_cast<std::uint32_t*>(type_record + 0x18))                                     // 0x0040132A
    {
        pooled = 1U;                                                                                   // 0x0040133A
        const std::uint32_t used = *reinterpret_cast<std::uint32_t*>(type_record + 0x14);
        const std::uint32_t* const slot_array =
            *reinterpret_cast<std::uint32_t* const*>(type_record + 0x10);                              // 0x0040132E
        const std::uint32_t slot = slot_array[used];                                                   // 0x0040133C
        const std::uintptr_t pool_base = *reinterpret_cast<std::uint32_t*>(type_record + 0x0C);         // 0x0040133F
        const std::uintptr_t address =
            static_cast<std::uintptr_t>(slot) *
                *reinterpret_cast<std::uint32_t*>(type_record + 0x1C) + pool_base;                     // 0x00401345/0x0040134C
        const std::uintptr_t store_base = *reinterpret_cast<const std::uintptr_t*>(type_record + 0x20);
        *reinterpret_cast<std::uint32_t*>(store_base + address) = used;                                 // 0x0040134E
        task = reinterpret_cast<std::uint8_t*>(address);
        *reinterpret_cast<std::uint32_t*>(type_record + 0x14) = used + 1U;                             // 0x00401351-0x00401358
    }
    else
    {
        pooled = 0U;                                                                                   // 0x00401360
        ::Mem_FlagStackPop(1U);                                                        // 0x00401362-0x00401364
        task = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(
            static_cast<std::int32_t>(*reinterpret_cast<std::uint32_t*>(type_record + 0x04)), 4U));    // 0x00401369-0x00401377
        ::Mem_FlagStackPush();                                                         // 0x0040137C
    }
    // The stores below must stay in the retail order: the live count was already incremented before any of them, so a
    // reader can observe this instance with a partially written header. Volatile keeps GCC from reordering them, the
    // same defect that was caught in the event allocator (stores emitted +2, +3, +0 instead of +2, word +0, +3).
    volatile std::uint8_t* const header = task;
    *reinterpret_cast<volatile std::uint32_t*>(const_cast<std::uint8_t*>(header) + 4) = 0U;              // 0x00401387
    *reinterpret_cast<volatile std::uint32_t*>(const_cast<std::uint8_t*>(header) + 0) = 0U;             // 0x0040138A
    *reinterpret_cast<volatile std::uint32_t*>(const_cast<std::uint8_t*>(header) + 0x14) = owner;       // 0x0040138C
    const std::uint8_t* const next_slot = task + 0xC;                                                   // 0x0040138F
    const std::uint8_t* const previous_slot = task + 8;                                                 // 0x00401392
    *const_cast<volatile std::uint8_t*>(header + 0x1E) = pooled;                                        // 0x00401395
    *reinterpret_cast<volatile std::uint16_t*>(const_cast<std::uint8_t*>(header) + 0x1C) =
        static_cast<std::uint16_t>(type_id);                                                            // 0x00401398
    *reinterpret_cast<volatile std::uint32_t*>(const_cast<std::uint8_t*>(header) + 0x18) = 0U;          // 0x0040139C
    *reinterpret_cast<volatile std::uint32_t*>(const_cast<std::uint8_t*>(previous_slot)) =
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(next_slot));                        // 0x0040139F
    *reinterpret_cast<volatile std::uint32_t*>(const_cast<std::uint8_t*>(next_slot)) = 0U;              // 0x004013A1
    *reinterpret_cast<volatile std::uint32_t*>(const_cast<std::uint8_t*>(header) + 0x10) =
        static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(previous_slot));                    // 0x004013A3
    return reinterpret_cast<igi::retail::input::InputPortRecord*>(task);                               // 0x004013A6
}

} // extern "C"

#undef NATIVE_DECL
