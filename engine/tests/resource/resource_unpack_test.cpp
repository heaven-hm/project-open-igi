#include "igi/retail/resource/resource_unpack.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

std::int32_t observed_device = -1;
std::int32_t observed_destination = 0;
std::int32_t observed_size = 0;

std::int32_t record_unpack(std::int32_t device, std::int32_t*, std::int32_t destination,
                           std::int32_t size)
{
    observed_device = device;
    observed_destination = destination;
    observed_size = size;
    return size;
}

// 0x004B16D0 dispatches through the device record selected by *context.
void test_dispatch_uses_the_selected_device()
{
    igi::retail::resource::Host_SetResourceUnpack(3, record_unpack);

    std::int32_t stream = 3;
    const std::int32_t result = igi::retail::resource::Resource_Unpack(&stream, 0x2000, 0x40);

    require(result == 0x40, "returns the device unpack result");
    require(observed_device == 3, "passes the device id from *context");
    require(observed_destination == 0x2000, "passes the destination");
    require(observed_size == 0x40, "passes the size");
}

// Each device id maps to its own 0x8C-byte record.
void test_devices_are_independent()
{
    igi::retail::resource::Host_SetResourceUnpack(1, nullptr);
    igi::retail::resource::Host_SetResourceUnpack(4, record_unpack);

    std::int32_t stream = 4;
    require(igi::retail::resource::Resource_Unpack(&stream, 0, 7) == 7, "device 4 uses its own entry");
}

} // namespace

int main()
{
    test_dispatch_uses_the_selected_device();
    test_devices_are_independent();

    std::cout << "All retail Resource_Unpack tests passed.\n";
    return EXIT_SUCCESS;
}
