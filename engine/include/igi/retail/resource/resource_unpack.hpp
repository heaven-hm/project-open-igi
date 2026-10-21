#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::resource {

// Retail device operation record: the unpack entry is the first field, and the device table
// (global 0x009436C0) advances 0x8C bytes per device id.
inline constexpr std::size_t resource_device_record_size = 0x8C;
inline constexpr std::size_t resource_device_count = 0x10;

using ResourceUnpackFn = std::int32_t (*)(std::int32_t device, std::int32_t* stream,
                                          std::int32_t destination, std::int32_t size);

// 0x004B16D0 - Resource_Unpack.
// Dispatches to the unpack entry of the device that `*context` selects.
std::int32_t Resource_Unpack(std::int32_t* context, std::int32_t destination, std::int32_t size);

// Host wiring for the port-owned device table. Not a retail entry point.
void Resource_SetUnpack(std::int32_t device, ResourceUnpackFn unpack);

} // namespace igi::retail::resource
