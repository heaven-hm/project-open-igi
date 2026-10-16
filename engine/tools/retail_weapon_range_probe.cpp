#include <windows.h>

#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

namespace
{

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void release_code(void* allocation)
{
    if (allocation != nullptr)
    {
        VirtualFree(allocation, 0, MEM_RELEASE);
    }
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        require(argument_count == 2, "Supply the original retail executable");
        std::ifstream input(arguments[1], std::ios::binary);
        require(static_cast<bool>(input), "Cannot read retail executable");
        const std::vector<char> image{std::istreambuf_iterator<char>(input), {}};
        require(image.size() > 0x7b26b, "Retail executable is truncated");
        constexpr std::size_t handler_size = 219;
        constexpr std::size_t constant_operand_offset = 0x24;
        const auto* original_handler = image.data() + 0x7b190;
        std::uint32_t original_constant_address = 0;
        std::memcpy(&original_constant_address, original_handler + constant_operand_offset, 4);
        require(static_cast<unsigned char>(original_handler[0x22]) == 0xd8 &&
                    static_cast<unsigned char>(original_handler[0x23]) == 0x0d &&
                    original_constant_address == 0x5333b8,
                "Retail FMUL instruction or constant address differs");
        const float world_units_per_metre = 4096.0f;
        std::unique_ptr<void, decltype(&release_code)> code(
            VirtualAlloc(nullptr, handler_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE), release_code);
        require(code != nullptr, "Cannot allocate private retail handler copy");
        std::memcpy(code.get(), original_handler, handler_size);
        const auto constant_address = reinterpret_cast<std::uintptr_t>(&world_units_per_metre);
        static_assert(sizeof(constant_address) == 4, "Build this analysis tool for Windows x86");
        std::memcpy(static_cast<char*>(code.get()) + constant_operand_offset, &constant_address, 4);
        for (std::size_t byte_index = 0; byte_index < handler_size; ++byte_index)
        {
            if (byte_index < constant_operand_offset || byte_index >= constant_operand_offset + 4)
            {
                require(static_cast<char*>(code.get())[byte_index] == original_handler[byte_index],
                        "Retail handler differs outside the constant address relocation");
            }
        }
        DWORD old_protection = 0;
        require(VirtualProtect(code.get(), handler_size, PAGE_EXECUTE_READ, &old_protection) != 0,
                "Cannot protect private handler code");
        require(FlushInstructionCache(GetCurrentProcess(), code.get(), handler_size) != 0,
                "Cannot flush handler instruction cache");
        using Handler = std::uint32_t(__cdecl*)(void*, const void*, const void*, const void*, int, int);
        const auto handler = reinterpret_cast<Handler>(code.get());
        std::size_t verified_ranges = 0;
        for (const float range_metres : {1.0f, 1.8f, 20.0f, 80.0f, 100.0f, 400.0f, 1000.0f, 2000.0f})
        {
            std::array<std::uint32_t, 400> shot_state{};
            std::array<std::uint32_t, 442> weapon_state{};
            const std::array<std::uint32_t, 6> transform{};
            const std::array<std::uint32_t, 3> direction{0, 0, 0x3f800000};
            weapon_state[0x650 / 4] = std::bit_cast<std::uint32_t>(range_metres);
            const auto result = handler(shot_state.data(), transform.data(), direction.data(),
                                        weapon_state.data(), 7, 11);
            require(result == 1 && shot_state[0x90 / 4] == 1 && shot_state[0x94 / 4] == 1,
                    "Retail shot slot allocation counters differ");
            require(shot_state[0xe0 / 4] == 1 &&
                        shot_state[(0xe0 + 0x34) / 4] ==
                            std::bit_cast<std::uint32_t>(range_metres * world_units_per_metre),
                    "Retail remaining range differs from original metres-to-units conversion");
            require(shot_state[(0xe0 + 0x44) / 4] == 11,
                    "Retail shot owner field differs");
            std::array<std::uint32_t, 400> expected_state{};
            expected_state[0x90 / 4] = 1;
            expected_state[0x94 / 4] = 1;
            expected_state[0xa4 / 4] = reinterpret_cast<std::uintptr_t>(&shot_state[0xe0 / 4]);
            expected_state[0xe0 / 4] = 1;
            expected_state[(0xe0 + 0x28) / 4] = 0x3f800000;
            expected_state[(0xe0 + 0x2c) / 4] = reinterpret_cast<std::uintptr_t>(weapon_state.data());
            expected_state[(0xe0 + 0x30) / 4] = 7;
            expected_state[(0xe0 + 0x34) / 4] =
                std::bit_cast<std::uint32_t>(range_metres * world_units_per_metre);
            expected_state[(0xe0 + 0x44) / 4] = 11;
            require(shot_state == expected_state,
                    "Retail handler changed unexpected fields in the shot object");
            ++verified_ranges;
        }
        std::cout << "Retail GunShot_TakeSlot executed for " << verified_ranges
                  << " ranges; range bits, counters, owner and active flag agree.\n"
                  << "Only the FMUL constant address was relocated; no game entrypoint executed.\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail weapon range probe failed: " << exception.what() << '\n';
        return 1;
    }
}
