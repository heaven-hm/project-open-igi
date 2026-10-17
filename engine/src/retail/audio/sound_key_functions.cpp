#include "igi/retail/audio/sound_key_functions.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace {

constexpr std::uint32_t retail_weight_mask = 0xFFFFU;
constexpr std::uint32_t retail_sound_key_hash_step = 0x13EU;
constexpr std::uint32_t retail_sound_key_hash_modulus = 0x65U;
constexpr std::uint32_t retail_name_hash_step = 0x13DU;

char uppercase_ascii_lowercase(char value) noexcept
{
    const auto unsigned_value = static_cast<unsigned char>(value);
    if (unsigned_value >= 0x61U && unsigned_value <= 0x7AU) {
        return static_cast<char>(unsigned_value - 0x20U);
    }

    return value;
}

} // namespace

extern "C" {

void IGI_RETAIL_CDECL Sound_ExtractNameFromPath(char* output, char* input_path)
{
    *output = '\0';

    char* first_dot = input_path;
    while (*first_dot != '\0' && *first_dot != '.') {
        ++first_dot;
    }

    if (*input_path == '\0' || *first_dot == '\0') {
        Engine_LogFormattedMessage("SOUND - Invalid filename %s", input_path);
        return;
    }

    std::ptrdiff_t scan_offset = first_dot - input_path;
    while (scan_offset >= 0) {
        const char path_character = input_path[scan_offset];
        if (path_character == '/' || path_character == '\\' || path_character == ':') {
            break;
        }
        --scan_offset;
    }

    if (scan_offset < 0) {
        return;
    }

    const auto key_start_offset = static_cast<std::size_t>(scan_offset + 1);
    const auto output_length = static_cast<std::size_t>(first_dot - input_path) - key_start_offset;
    for (std::size_t byte_index = 0; byte_index < output_length; ++byte_index) {
        output[byte_index] = input_path[key_start_offset + byte_index];
    }
    output[output_length] = '\0';
}

std::int32_t IGI_RETAIL_CDECL Sound_ComputeRollingNameHash(char* name)
{
    // Listing-derived register result; Ghidra pseudocode declares this routine void.
    std::uint32_t hash_remainder = 0U;
    std::uint32_t character_weight = 1U;

    while (*name != '\0') {
        *name = uppercase_ascii_lowercase(*name);
        const auto byte_value = static_cast<unsigned char>(*name);
        const auto signed_byte_value = std::bit_cast<std::int8_t>(byte_value);
        const auto signed_contribution = static_cast<std::int32_t>(signed_byte_value)
            * static_cast<std::int32_t>(character_weight);
        const std::uint32_t dividend = hash_remainder
            + static_cast<std::uint32_t>(signed_contribution);

        hash_remainder = dividend % retail_sound_key_hash_modulus;
        character_weight = (character_weight + retail_sound_key_hash_step) & retail_weight_mask;
        ++name;
    }

    return static_cast<std::int32_t>(hash_remainder);
}

std::int32_t IGI_RETAIL_CDECL Sound_ComputeWeightedNameHash(char* name)
{
    std::uint32_t hash_value = 0U;
    std::uint32_t character_weight = 1U;

    while (*name != '\0') {
        *name = uppercase_ascii_lowercase(*name);
        const auto byte_value = static_cast<unsigned char>(*name);
        const auto signed_byte_value = std::bit_cast<std::int8_t>(byte_value);
        const auto signed_contribution = static_cast<std::int32_t>(signed_byte_value)
            * static_cast<std::int32_t>(character_weight);

        hash_value += static_cast<std::uint32_t>(signed_contribution);
        character_weight = (character_weight + retail_name_hash_step) & retail_weight_mask;
        ++name;
    }

    return std::bit_cast<std::int32_t>(hash_value);
}

} // extern "C"

#undef IGI_RETAIL_CDECL
