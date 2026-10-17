#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

std::int32_t g_handler_calls = 0;
std::uint32_t g_last_handler_size = 0U;

std::int32_t test_new_handler(std::uint32_t size)
{
    g_handler_calls += 1;
    g_last_handler_size = size;
    return g_handler_calls == 1 ? 1 : 0;
}

void test_qhash_value_set()
{
    igi::retail::mem::Mem_ResetGlobalsForTest();
    require(igi::retail::mem::Mem_GetGlobals().qhash_flag_9422fd == 0U,
            "qhash_flag_9422fd starts at 0");
    igi::retail::mem::QHash_ValueSet();
    require(igi::retail::mem::Mem_GetGlobals().qhash_flag_9422fd == 1U,
            "QHash_ValueSet sets 0x009422FD to 1");
}

void test_new_handler_and_widescreen_threshold()
{
    igi::retail::mem::Mem_ResetGlobalsForTest();
    g_handler_calls = 0;
    g_last_handler_size = 0U;

    require(igi::retail::mem::Camera_ApplyHandiCam_28(64U) == 0,
            "Camera_ApplyHandiCam_28 returns 0 when new_handler is null");

    igi::retail::mem::Mem_GetGlobals().new_handler_936254 = &test_new_handler;
    require(igi::retail::mem::Camera_ApplyHandiCam_28(128U) == 1,
            "Camera_ApplyHandiCam_28 returns 1 when handler returns non-zero");
    require(g_last_handler_size == 128U, "handler receives requested size");
    require(igi::retail::mem::Camera_ApplyHandiCam_28(128U) == 0,
            "Camera_ApplyHandiCam_28 returns 0 when handler returns zero");

    require(igi::retail::mem::Camera_ApplyWideScreen_20(0xFFFFFFE1U, 0) == nullptr,
            "Camera_ApplyWideScreen_20 rejects sizes above 0xFFFFFFE0");

    void* block = igi::retail::mem::Camera_ApplyHandiCam_20(32U);
    require(block != nullptr, "Camera_ApplyHandiCam_20 allocates valid size");
    std::free(block);
}

void test_mem_alloc_alignment_and_globals()
{
    igi::retail::mem::Mem_ResetGlobalsForTest();
    auto& globals = igi::retail::mem::Mem_GetGlobals();

    void* const p1 = igi::retail::mem::Mem_Alloc(64, 16);
    require(p1 != nullptr, "Mem_Alloc returns non-null payload");
    require((reinterpret_cast<std::uintptr_t>(p1) & 15U) == 0U,
            "Mem_Alloc payload is 16-byte aligned");
    require(globals.alloc_count_942300 == 1, "Mem_Alloc increments 0x00942300");
    require(igi::retail::mem::g_942300 == 1, "g_942300 alias matches alloc_count_942300");
    require(globals.heap_init_9422fc == 1U, "Mem_Alloc lazily sets 0x009422FC");
    require(igi::retail::mem::g_9422fc == 1U, "g_9422fc alias matches heap_init_9422fc");
    require(igi::retail::mem::g_a960f0 != nullptr, "g_a960f0 points to allocated block node");
    require(globals.qhash_flag_9422fd == 1U, "Mem_Alloc lazy init sets 0x009422FD");
    require(globals.state_a960e0 == 0x10, "Mem_Alloc lazy init sets 0x00A960E0 = 0x10");

    const std::uintptr_t result_addr = reinterpret_cast<std::uintptr_t>(p1);
    const std::uintptr_t backptr_addr =
        (result_addr - sizeof(void*)) & ~(sizeof(void*) - 1U);
    auto** const raw_node = *reinterpret_cast<void***>(backptr_addr);
    require(raw_node != nullptr, "back-pointer stores raw header address");
    require(raw_node == igi::retail::mem::g_a960f0,
            "g_a960f0 head points at the newly allocated raw block header");
    require(raw_node[2] == p1, "raw[2] stores aligned payload pointer");
    require(reinterpret_cast<std::intptr_t>(raw_node[3]) == 64 + 16 + 0x1C,
            "raw[3] stores total requested size");
    require(reinterpret_cast<std::intptr_t>(raw_node[4]) == 64,
            "raw[4] stores user byte count");
}

} // namespace

int main()
{
    test_qhash_value_set();
    test_new_handler_and_widescreen_threshold();
    test_mem_alloc_alignment_and_globals();
    std::cout << "All retail Mem_Alloc tests passed.\n";
    return EXIT_SUCCESS;
}

