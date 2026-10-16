#include "igi/retail_random_generator.hpp"

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

void release_memory(void* allocation)
{
    if (allocation != nullptr)
    {
        VirtualFree(allocation, 0, MEM_RELEASE);
    }
}

using Allocation = std::unique_ptr<void, decltype(&release_memory)>;

Allocation copy_instructions(const std::vector<char>& image, std::size_t offset, std::size_t size,
                             const double* relocated_scale = nullptr)
{
    require(offset <= image.size() && size <= image.size() - offset, "Truncated retail routine");
    Allocation allocation(VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE),
                          release_memory);
    require(allocation != nullptr, "Cannot allocate private retail routine");
    std::memcpy(allocation.get(), image.data() + offset, size);
    constexpr std::size_t constant_operand_offset = 0x3f;
    if (relocated_scale != nullptr)
    {
        require(size >= constant_operand_offset + 4, "Truncated scale operand");
        std::uint32_t original_address = 0;
        std::memcpy(&original_address, image.data() + offset + constant_operand_offset, 4);
        require(original_address == 0x534aa8, "Original RNG constant operand differs");
        const auto scale_address = reinterpret_cast<std::uintptr_t>(relocated_scale);
        std::memcpy(static_cast<char*>(allocation.get()) + constant_operand_offset, &scale_address, 4);
    }
    DWORD old_protection = 0;
    require(VirtualProtect(allocation.get(), size, PAGE_EXECUTE_READ, &old_protection) != 0,
            "Cannot protect private retail routine");
    require(FlushInstructionCache(GetCurrentProcess(), allocation.get(), size) != 0,
            "Cannot flush retail instruction cache");
    for (std::size_t byte_index = 0; byte_index < size; ++byte_index)
    {
        if (relocated_scale != nullptr && byte_index >= constant_operand_offset &&
            byte_index < constant_operand_offset + 4)
        {
            continue;
        }
        require(static_cast<const char*>(allocation.get())[byte_index] == image[offset + byte_index],
                "Original instruction bytes changed outside constant relocation");
    }
    return allocation;
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        static_assert(sizeof(void*) == 4, "Build this analysis tool for Windows x86");
        require(argument_count == 2, "Supply original retail executable");
        std::ifstream input(arguments[1], std::ios::binary);
        require(static_cast<bool>(input), "Cannot read retail executable");
        const std::vector<char> image{std::istreambuf_iterator<char>(input), {}};
        require(image.size() >= 0x134ab0, "Truncated original random scale");
        double scale = 0;
        std::memcpy(&scale, image.data() + 0x134aa8, sizeof(scale));
        require(scale == 0.0000152587890625, "Original random scale does not match");

        const auto initializer_code = copy_instructions(image, 0xb4720, 25);
        const auto word_code = copy_instructions(image, 0xb4740, 38);
        const auto single_code = copy_instructions(image, 0xb4770, 71, &scale);
        using Initialize = void(__cdecl*)(std::uint32_t*);
        using NextWord = std::uint32_t(__cdecl*)(std::uint32_t*);
        using NextReal = double(__cdecl*)(std::uint32_t*);
        const auto initialize = reinterpret_cast<Initialize>(initializer_code.get());
        const auto next_word = reinterpret_cast<NextWord>(word_code.get());
        const auto next_real = reinterpret_cast<NextReal>(single_code.get());

        std::array<std::uint32_t, 3> original_default{};
        initialize(original_default.data());
        require(original_default == igi::gameplay::RetailRandomGenerator{}.state(),
                "Original initializer differs from port");
        const std::array<std::array<std::uint32_t, 3>, 6> initial_states{
            original_default, std::array<std::uint32_t, 3>{0, 0, 0},
            {1, 2, 3}, {0xffffffffu, 0xffffffffu, 0xffffffffu},
            {0x80000000u, 0, 0}, {0, 0x80000000u, 0xffffffffu}};
        std::size_t checked_draws = 0;
        for (const auto& initial_state : initial_states)
        {
            auto original_words = initial_state;
            auto original_reals = initial_state;
            igi::gameplay::RetailRandomGenerator port_words(initial_state);
            igi::gameplay::RetailRandomGenerator port_reals(initial_state);
            for (std::size_t draw_index = 0; draw_index < 65536; ++draw_index)
            {
                const auto original_word = next_word(original_words.data());
                const float original_real = static_cast<float>(next_real(original_reals.data()));
                require(original_word == port_words.next_word() && original_words == port_words.state(),
                        "Original word draw or memory state differs from port");
                require(std::bit_cast<std::uint32_t>(original_real) ==
                            std::bit_cast<std::uint32_t>(port_reals.next_single()) &&
                            original_reals == port_reals.state(),
                        "Original float draw or memory state differs from port");
                if (initial_state == original_default && draw_index < 8)
                {
                    std::cout << "draw=" << draw_index << " word=" << original_word
                              << " float_bits=" << std::bit_cast<std::uint32_t>(original_real) << '\n';
                }
                ++checked_draws;
            }
        }
        std::cout << "Retail RNG differential passed: " << checked_draws
                  << " paired word/float draws; all three state words match after every draw.\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail RNG differential failed: " << exception.what() << '\n';
        return 1;
    }
}
