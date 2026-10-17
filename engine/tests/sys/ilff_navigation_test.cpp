#include "igi/retail/sys/ilff_navigation.hpp"

#include <array>
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

void write_i32(std::array<std::uint8_t, 0x100>& buffer, std::size_t offset, std::uint32_t value)
{
    buffer[offset + 0] = static_cast<std::uint8_t>(value & 0xFFU);
    buffer[offset + 1] = static_cast<std::uint8_t>((value >> 8U) & 0xFFU);
    buffer[offset + 2] = static_cast<std::uint8_t>((value >> 16U) & 0xFFU);
    buffer[offset + 3] = static_cast<std::uint8_t>((value >> 24U) & 0xFFU);
}

// Parent at 0x00: size 0x40, align 4. First child at 0x14 (tag ATTA), second at 0x24 (tag DNER).
std::array<std::uint8_t, 0x100> make_container()
{
    std::array<std::uint8_t, 0x100> buffer{};
    write_i32(buffer, 0x00, 0x4D455348U); // MESH
    write_i32(buffer, 0x04, 0x40U);
    write_i32(buffer, 0x08, 4U);
    write_i32(buffer, 0x0C, 0U); // no next sibling

    write_i32(buffer, 0x14, 0x41545441U); // ATTA
    write_i32(buffer, 0x18, 0x10U);
    write_i32(buffer, 0x1C, 4U);
    write_i32(buffer, 0x20, 0x10U); // next sibling at 0x24

    write_i32(buffer, 0x24, 0x52454E44U); // DNER
    write_i32(buffer, 0x28, 0x08U);
    write_i32(buffer, 0x2C, 4U);
    write_i32(buffer, 0x30, 0U); // chain end
    return buffer;
}

// 0x004B7470
void test_pop_stack_follows_the_relative_next_offset()
{
    auto buffer = make_container();
    void* base = buffer.data();
    require(igi::retail::sys::Sys_PopStack_19(base) == nullptr,
            "a chunk with next offset 0 has no sibling");
    require(igi::retail::sys::Sys_PopStack_19(buffer.data() + 0x14) == buffer.data() + 0x24,
            "the child's next offset 0x10 resolves to 0x24");
    require(igi::retail::sys::Sys_PopStack_19(nullptr) == nullptr, "null input returns null");
}

// 0x004B7490
void test_resolve_symbol_returns_the_first_child_position()
{
    auto buffer = make_container();
    require(igi::retail::sys::Sys_ResolveSymbol_19(buffer.data()) == buffer.data() + 0x14,
            "(4 + 0x13) & ~3 == 0x14");
}

// 0x004B74D0
void test_check_heap_resolves_the_child_payload()
{
    auto buffer = make_container();
    void* payload = igi::retail::sys::Sys_CheckHeap_19(buffer.data() + 0x14, buffer.data());
    require(payload == buffer.data() + 0x24, "payload = parent + ((4 + 0x14 + 0xF) & ~3)");
}

// 0x004B7590
void test_read_file_block_walks_the_sibling_chain_by_tag()
{
    auto buffer = make_container();
    auto* context = reinterpret_cast<std::int32_t*>(buffer.data() + 0x14);
    require(igi::retail::sys::Sys_ReadFileBlock_20(context, 0x41545441) == context,
            "the first sibling matches its own tag");
    require(igi::retail::sys::Sys_ReadFileBlock_20(context, 0x52454E44)
                == reinterpret_cast<std::int32_t*>(buffer.data() + 0x24),
            "the walk reaches the second sibling by tag");
    require(igi::retail::sys::Sys_ReadFileBlock_20(context, 0x4E415854) == nullptr,
            "an absent tag returns null");
}

// 0x004B7540, 0x004B7570, 0x004B7580
void test_chunk_header_accessors()
{
    auto buffer = make_container();
    write_i32(buffer, 0x10, 0x53455249U); // IRES form at +0x10

    std::int32_t size = 0;
    std::int32_t align = 0;
    igi::retail::sys::Sys_AllocateMemory_20(&size, &align, buffer.data());
    require(size == 0x40, "Sys_AllocateMemory_20 reads declared size at +0x04");
    require(align == 4, "Sys_AllocateMemory_20 reads alignment at +0x08");
    igi::retail::sys::Sys_AllocateMemory_20(nullptr, nullptr, nullptr);

    require(igi::retail::sys::Sys_FreeMemory_20(buffer.data()) == 0x53455249,
            "Sys_FreeMemory_20 reads form FourCC at +0x10");
    require(igi::retail::sys::Sys_FreeMemory_20(nullptr) == 0,
            "Sys_FreeMemory_20 returns 0 on null");

    const auto* tag_ptr = reinterpret_cast<const std::int32_t*>(buffer.data());
    require(igi::retail::sys::Sys_OpenFileStream_20(tag_ptr) == 0x4D455348,
            "Sys_OpenFileStream_20 reads tag FourCC at +0x00");
    require(igi::retail::sys::Sys_OpenFileStream_20(nullptr) == 0,
            "Sys_OpenFileStream_20 returns 0 on null");
}

} // namespace

int main()
{
    test_pop_stack_follows_the_relative_next_offset();
    test_resolve_symbol_returns_the_first_child_position();
    test_check_heap_resolves_the_child_payload();
    test_read_file_block_walks_the_sibling_chain_by_tag();
    test_chunk_header_accessors();

    std::cout << "All retail ILFF navigation tests passed.\n";
    return EXIT_SUCCESS;
}
