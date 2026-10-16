#include "igi/retail_random_generator.hpp"

#include <array>
#include <bit>
#include <exception>
#include <iostream>
#include <stdexcept>

extern "C"
{
#if defined(_MSC_VER)
__declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
__attribute__((noinline))
#endif
std::uint32_t igi_retail_random_draw(igi::gameplay::RetailRandomGenerator* generator)
{
    return generator->next_word();
}
}

int main()
{
    try
    {
        constexpr std::array<std::uint32_t, 8> original_words{
            277206966u, 1246844079u, 2440000437u, 3394835923u,
            283362048u, 1236019352u, 2412125716u, 3382577080u};
        constexpr std::array<std::uint32_t, 8> original_single_bits{
            1062712832u, 1051287040u, 1055353344u, 1034852352u,
            1061355520u, 1042440192u, 1039179776u, 1022820352u};
        igi::gameplay::RetailRandomGenerator words;
        igi::gameplay::RetailRandomGenerator singles;
        for (std::size_t draw_index = 0; draw_index < original_words.size(); ++draw_index)
        {
            if (igi_retail_random_draw(&words) != original_words[draw_index] ||
                std::bit_cast<std::uint32_t>(singles.next_single()) != original_single_bits[draw_index] ||
                words.state() != singles.state())
            {
                throw std::runtime_error("RNG differs from captured original x86 draw " +
                                         std::to_string(draw_index));
            }
        }
        igi::gameplay::RetailRandomGenerator restored(words.state());
        for (std::size_t draw_index = 0; draw_index < 4096; ++draw_index)
        {
            if (words.next_word() != restored.next_word() || words.state() != restored.state())
            {
                throw std::runtime_error("Captured RNG state does not replay deterministically");
            }
        }
        igi::gameplay::RetailRandomGenerator zero(std::array<std::uint32_t, 3>{});
        if (zero.next_word() != 0 || zero.next_single() != 0)
        {
            throw std::runtime_error("Zero-state behavior differs from original recurrence");
        }
        std::cout << "Retail RNG captured checkpoints and state replay passed\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail RNG checkpoint failed: " << exception.what() << '\n';
        return 1;
    }
}
