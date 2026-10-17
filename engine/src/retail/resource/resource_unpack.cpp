#include "igi/retail/resource/resource_unpack.hpp"

#include <cstring>

namespace igi::retail::resource {
namespace {

// Port-owned storage for the retail device table at 0x009436C0.
std::uint8_t device_table[resource_device_count * resource_device_record_size]{};

} // namespace

// 0x004B16D0
std::int32_t Resource_Unpack(std::int32_t* context, std::int32_t destination, std::int32_t size)
{
    const std::int32_t device = *context;
    ResourceUnpackFn unpack = nullptr;
    std::memcpy(&unpack,
                device_table + static_cast<std::size_t>(device) * resource_device_record_size,
                sizeof(unpack));
    return unpack(device, context, destination, size);
}

void Host_SetResourceUnpack(std::int32_t device, ResourceUnpackFn unpack)
{
    std::memcpy(device_table + static_cast<std::size_t>(device) * resource_device_record_size,
                &unpack, sizeof(unpack));
}

} // namespace igi::retail::resource
