#include "igi/retail/audio/sound_attenuation_table.hpp"

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

void test_table_matches_retail_silence_and_full_scale_entries()
{
    const auto attenuation_table = igi::retail::audio::Sound_BuildAttenuationTable_004952F0();

    require(attenuation_table.size() == 10001U, "retail table contains 10001 entries");
    require(attenuation_table[0U] == -10000, "retail writes the silence floor at entry zero");
    require(attenuation_table[10000U] == 0, "unity gain maps to zero attenuation");
}

void test_table_uses_x87_rounding_at_exact_gain_halves()
{
    const auto attenuation_table = igi::retail::audio::Sound_BuildAttenuationTable_004952F0();

    require(attenuation_table[5000U] == -1000, "half gain rounds to the retail -1000 value");
    require(attenuation_table[2500U] == -2000, "quarter gain rounds to the retail -2000 value");
    require(attenuation_table[1250U] == -3000, "eighth gain rounds to the retail -3000 value");
}

void test_attenuation_table_is_monotonic_and_clamped()
{
    const auto attenuation_table = igi::retail::audio::Sound_BuildAttenuationTable_004952F0();

    for (std::size_t gain_index = 1U; gain_index < attenuation_table.size(); ++gain_index) {
        require(attenuation_table[gain_index] >= -10000, "attenuation does not exceed the silence floor");
        require(attenuation_table[gain_index] <= 0, "attenuation does not exceed full scale");
        require(
            attenuation_table[gain_index] >= attenuation_table[gain_index - 1U],
            "attenuation is monotonic as gain increases");
    }
}

} // namespace

int main()
{
    test_table_matches_retail_silence_and_full_scale_entries();
    test_table_uses_x87_rounding_at_exact_gain_halves();
    test_attenuation_table_is_monotonic_and_clamped();

    std::cout << "Retail attenuation table tests passed.\n";
    return EXIT_SUCCESS;
}
