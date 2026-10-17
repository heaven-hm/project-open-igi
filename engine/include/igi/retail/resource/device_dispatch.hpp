#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::resource {

// Device tables used by the resource stream helpers. Each table advances 0x8C bytes per device
// id, and the entry at offset 0 of the selected record is the operation pointer:
//   Sys_ReadFileBlock_4 -> 0x009436BC
//   Sys_PushStack_4     -> 0x009436D0
//   Sys_CheckHeap_4     -> 0x009436E4
inline constexpr std::size_t dispatch_record_size = 0x8C;
inline constexpr std::size_t dispatch_device_count = 0x10;

// Native: Device_GetStreamPosition() - VA 0x004B1780 - complete port.
// Previous catalog alias: Sys_PushStack_4.
std::int32_t Device_GetStreamPosition(std::int32_t* context);
inline std::int32_t Sys_PushStack_4(std::int32_t* context)
{
    return Device_GetStreamPosition(context);
}

// Native: Device_SeekStream() - VA 0x004B17F0 - complete port.
// Previous catalog alias: Sys_CheckHeap_4.
std::int32_t Device_SeekStream(std::int32_t* context, std::int32_t argument);
inline std::int32_t Sys_CheckHeap_4(std::int32_t* context, std::int32_t argument)
{
    return Device_SeekStream(context, argument);
}

// Native: Device_CloseStream() - VA 0x004B1690 - complete port.
// Previous catalog alias: Sys_ReadFileBlock_4.
// The retail global stream at 0x00943980 is a special case: it clears 0x00943988 instead of
// dispatching.
void Device_CloseStream(std::int32_t* context);
inline void Sys_ReadFileBlock_4(std::int32_t* context)
{
    Device_CloseStream(context);
}

// Port-owned storage for the retail global stream compared by Sys_ReadFileBlock_4.
[[nodiscard]] std::int32_t* Resource_GetGlobalStream();
[[nodiscard]] std::int32_t Resource_GetGlobalStreamFlag();

// Host wiring for the port-owned operation tables. Not retail entry points.
using DispatchPushStackFn = std::int32_t (*)(std::int32_t device, std::int32_t* context);
using DispatchCheckHeapFn = std::int32_t (*)(std::int32_t device, std::int32_t* context,
                                             std::int32_t argument);
using DispatchReadFileBlockFn = void (*)(std::int32_t device, std::int32_t* context);

void Host_SetPushStack(std::int32_t device, DispatchPushStackFn operation);
void Host_SetCheckHeap(std::int32_t device, DispatchCheckHeapFn operation);
void Host_SetReadFileBlock(std::int32_t device, DispatchReadFileBlockFn operation);

} // namespace igi::retail::resource
