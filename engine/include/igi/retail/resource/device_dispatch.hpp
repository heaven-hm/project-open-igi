#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::resource {

// Device tables used by the resource stream helpers. Each table advances 0x8C bytes per device
// id, and the entry at offset 0 of the selected record is the operation pointer:
//   Device_CloseStream -> 0x009436BC
//   Device_GetStreamPosition     -> 0x009436D0
//   Device_SeekStream     -> 0x009436E4
inline constexpr std::size_t dispatch_record_size = 0x8C;
inline constexpr std::size_t dispatch_device_count = 0x10;

// Native: Device_GetStreamPosition() - VA 0x004B1780 - complete port.
std::int32_t Device_GetStreamPosition(std::int32_t* context);

// Native: Device_SeekStream() - VA 0x004B17F0 - complete port.
std::int32_t Device_SeekStream(std::int32_t* context, std::int32_t argument);

// Native: Device_CloseStream() - VA 0x004B1690 - complete port.
// The retail global stream at 0x00943980 is a special case: it clears 0x00943988 instead of
// dispatching.
void Device_CloseStream(std::int32_t* context);

// Port-owned storage for the retail global stream compared by Device_CloseStream.
[[nodiscard]] std::int32_t* Resource_GetGlobalStream();
[[nodiscard]] std::int32_t Resource_GetGlobalStreamFlag();

// Host wiring for the port-owned operation tables. Not retail entry points.
using DispatchPushStackFn = std::int32_t (*)(std::int32_t device, std::int32_t* context);
using DispatchCheckHeapFn = std::int32_t (*)(std::int32_t device, std::int32_t* context,
                                             std::int32_t argument);
using DispatchReadFileBlockFn = void (*)(std::int32_t device, std::int32_t* context);

void Device_SetPushStack(std::int32_t device, DispatchPushStackFn operation);
void Device_SetCheckHeap(std::int32_t device, DispatchCheckHeapFn operation);
void Device_SetReadFileBlock(std::int32_t device, DispatchReadFileBlockFn operation);

} // namespace igi::retail::resource
