#include "igi/retail/resource/device_dispatch.hpp"

#include <cstring>

namespace igi::retail::resource {
namespace {

std::uint8_t push_stack_table[dispatch_device_count * dispatch_record_size]{};
std::uint8_t check_heap_table[dispatch_device_count * dispatch_record_size]{};
std::uint8_t read_file_block_table[dispatch_device_count * dispatch_record_size]{};

// 0x00943980 (stream) and 0x00943988 (flag).
std::int32_t global_stream[0x10]{};
std::int32_t global_stream_flag = 0;

template <class Function>
[[nodiscard]] Function load_operation(const std::uint8_t* table, std::int32_t device)
{
    Function operation = nullptr;
    std::memcpy(&operation,
                table + static_cast<std::size_t>(device) * dispatch_record_size,
                sizeof(operation));
    return operation;
}

template <class Function>
void store_operation(std::uint8_t* table, std::int32_t device, Function operation)
{
    std::memcpy(table + static_cast<std::size_t>(device) * dispatch_record_size,
                &operation, sizeof(operation));
}

} // namespace

// Native: Device_GetStreamPosition; retail VA 0x004B1780; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
std::int32_t Device_GetStreamPosition(std::int32_t* context)
{
    const std::int32_t device = *context;
    const DispatchPushStackFn operation = load_operation<DispatchPushStackFn>(push_stack_table, device);
    return operation(device, context);
}

// Native: Device_SeekStream; retail VA 0x004B17F0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
std::int32_t Device_SeekStream(std::int32_t* context, std::int32_t argument)
{
    const std::int32_t device = *context;
    const DispatchCheckHeapFn operation = load_operation<DispatchCheckHeapFn>(check_heap_table, device);
    return operation(device, context, argument);
}

// Native: Device_CloseStream; retail VA 0x004B1690; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail igi.exe.
// Behavior/CFG: not restated here.
// Status/limits: STRUCTURALLY_VERIFIED; see verification record for scope and port limits.
void Device_CloseStream(std::int32_t* context)
{
    if (context == global_stream)
    {
        global_stream_flag = 0;
        return;
    }

    const std::int32_t device = *context;
    const DispatchReadFileBlockFn operation =
        load_operation<DispatchReadFileBlockFn>(read_file_block_table, device);
    operation(device, context);
}

std::int32_t* Resource_GetGlobalStream()
{
    return global_stream;
}

std::int32_t Resource_GetGlobalStreamFlag()
{
    return global_stream_flag;
}

void Device_SetPushStack(std::int32_t device, DispatchPushStackFn operation)
{
    store_operation(push_stack_table, device, operation);
}

void Device_SetCheckHeap(std::int32_t device, DispatchCheckHeapFn operation)
{
    store_operation(check_heap_table, device, operation);
}

void Device_SetReadFileBlock(std::int32_t device, DispatchReadFileBlockFn operation)
{
    store_operation(read_file_block_table, device, operation);
}

} // namespace igi::retail::resource
