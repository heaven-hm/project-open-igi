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

// Native: Device_GetStreamPosition() - VA 0x004B1780 - complete port.
// Previous catalog alias: Sys_PushStack_4.
std::int32_t Device_GetStreamPosition(std::int32_t* context)
{
    const std::int32_t device = *context;
    const DispatchPushStackFn operation = load_operation<DispatchPushStackFn>(push_stack_table, device);
    return operation(device, context);
}

// Native: Device_SeekStream() - VA 0x004B17F0 - complete port.
// Previous catalog alias: Sys_CheckHeap_4.
std::int32_t Device_SeekStream(std::int32_t* context, std::int32_t argument)
{
    const std::int32_t device = *context;
    const DispatchCheckHeapFn operation = load_operation<DispatchCheckHeapFn>(check_heap_table, device);
    return operation(device, context, argument);
}

// Native: Device_CloseStream() - VA 0x004B1690 - complete port.
// Previous catalog alias: Sys_ReadFileBlock_4.
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

void Host_SetPushStack(std::int32_t device, DispatchPushStackFn operation)
{
    store_operation(push_stack_table, device, operation);
}

void Host_SetCheckHeap(std::int32_t device, DispatchCheckHeapFn operation)
{
    store_operation(check_heap_table, device, operation);
}

void Host_SetReadFileBlock(std::int32_t device, DispatchReadFileBlockFn operation)
{
    store_operation(read_file_block_table, device, operation);
}

} // namespace igi::retail::resource
