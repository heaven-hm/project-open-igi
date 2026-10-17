#include "igi/retail/resource/device_dispatch.hpp"

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

std::int32_t push_device = -1;
std::int32_t check_device = -1;
std::int32_t check_argument = 0;
std::int32_t read_device = -1;

std::int32_t record_push(std::int32_t device, std::int32_t*) { push_device = device; return device + 1; }
std::int32_t record_check(std::int32_t device, std::int32_t*, std::int32_t argument)
{ check_device = device; check_argument = argument; return argument * 2; }
void record_read(std::int32_t device, std::int32_t*) { read_device = device; }

// 0x004B1780
void test_push_stack_dispatch()
{
    igi::retail::resource::Host_SetPushStack(2, record_push);
    std::int32_t stream = 2;
    require(igi::retail::resource::Sys_PushStack_4(&stream) == 3, "returns the operation result");
    require(push_device == 2, "passes the device id");
}

// 0x004B17F0
void test_check_heap_dispatch()
{
    igi::retail::resource::Host_SetCheckHeap(1, record_check);
    std::int32_t stream = 1;
    require(igi::retail::resource::Sys_CheckHeap_4(&stream, 0x21) == 0x42, "returns the operation result");
    require(check_device == 1 && check_argument == 0x21, "passes device and argument");
}

// 0x004B1690, including the 0x00943980 special case
void test_read_file_block_dispatch_and_global_stream()
{
    igi::retail::resource::Host_SetReadFileBlock(5, record_read);
    std::int32_t stream = 5;
    igi::retail::resource::Sys_ReadFileBlock_4(&stream);
    require(read_device == 5, "dispatches to the device operation");

    igi::retail::resource::Sys_ReadFileBlock_4(igi::retail::resource::Resource_GetGlobalStream());
    require(igi::retail::resource::Resource_GetGlobalStreamFlag() == 0,
            "the global stream clears 0x00943988 instead of dispatching");
    require(read_device == 5, "the global stream does not reach the device operation");
}

} // namespace

int main()
{
    test_push_stack_dispatch();
    test_check_heap_dispatch();
    test_read_file_block_dispatch_and_global_stream();

    std::cout << "All retail device-dispatch tests passed.\n";
    return EXIT_SUCCESS;
}
