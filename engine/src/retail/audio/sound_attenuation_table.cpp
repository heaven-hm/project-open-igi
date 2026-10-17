#include "igi/retail/audio/sound_attenuation_table.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <cstdint>

namespace igi::retail::audio {
namespace {

// Original IGI.EXE FUN_004952F0 - 0x004952F0 - sound initialization / attenuation table.
// Evidence: REA/Ghidra assembly ev_09cd5938501ba5afe5ad535f2bbe0d0849ae946f5d340cbaf82b755cd0c905e6,
// decompilation ev_1fb1bd1433efcce21bfb436e786a22441e9d0006bdd8f31d99d7222a0809f682,
// and retail constant bytes ev_4d34bb6fb909169027530f475d5163f3fd105e68fed3dce68bed71b729cc6a7a /
// ev_e225bf5feb527ce9e38770b04e632a75e25d49fc3dde4c80fa0ff920fc58ed4d.
constexpr std::uint64_t retail_gain_quantum_bits = 0x3F1A36E2EB1C432DULL;
constexpr std::uint64_t retail_attenuation_scale_bits = 0x40968AC7B8BC7BF7ULL;
constexpr double retail_gain_quantum = std::bit_cast<double>(retail_gain_quantum_bits);
constexpr double retail_attenuation_scale = std::bit_cast<double>(retail_attenuation_scale_bits);
constexpr std::int32_t retail_silence_attenuation = -10000;

[[nodiscard]] std::int64_t retail_x87_attenuation_for_index(std::int32_t gain_index) noexcept
{
    std::int64_t rounded_attenuation = 0;

#if defined(_MSC_VER) && defined(_M_IX86)
    __asm {
        fild dword ptr [gain_index]
        fmul qword ptr [retail_gain_quantum]
        fldln2
        fxch
        fyl2x
        fmul qword ptr [retail_attenuation_scale]
        fistp qword ptr [rounded_attenuation]
    }
#elif defined(__i386__)
    __asm__ volatile(
        "fildl %[gain_index]\n\t"
        "fmull %[gain_quantum]\n\t"
        "fldln2\n\t"
        "fxch\n\t"
        "fyl2x\n\t"
        "fmull %[attenuation_scale]\n\t"
        "fistpq %[rounded_attenuation]"
        : [rounded_attenuation] "=m" (rounded_attenuation)
        : [gain_index] "m" (gain_index),
          [gain_quantum] "m" (retail_gain_quantum),
          [attenuation_scale] "m" (retail_attenuation_scale)
        : "st");
#else
    // Host-only fallback for analysis builds. Retail parity is provided by the x87 path above.
    const double gain = static_cast<double>(gain_index) * retail_gain_quantum;
    const double attenuation = std::log(gain) * retail_attenuation_scale;
    rounded_attenuation = static_cast<std::int64_t>(std::nearbyint(attenuation));
#endif

    return rounded_attenuation;
}

} // namespace

SoundAttenuationTable Sound_BuildAttenuationTable_004952F0()
{
    SoundAttenuationTable attenuation_table{};
    attenuation_table[0U] = retail_silence_attenuation;

    // Retail fills entries from gain 1.0000 down to 0.0001. Each input is formed by
    // FILD(index) followed by FMUL using the exact double stored at 0x00534268.
    for (std::int32_t gain_index = 10000; gain_index > 0; --gain_index) {
        const std::int64_t rounded_attenuation = retail_x87_attenuation_for_index(gain_index);
        const std::int64_t clamped_attenuation = std::clamp<std::int64_t>(
            rounded_attenuation,
            retail_silence_attenuation,
            0);
        attenuation_table[static_cast<std::size_t>(gain_index)] =
            static_cast<std::int32_t>(clamped_attenuation);
    }

    return attenuation_table;
}

} // namespace igi::retail::audio
