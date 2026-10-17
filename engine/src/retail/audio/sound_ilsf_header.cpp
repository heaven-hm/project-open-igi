#include "igi/retail/audio/sound_ilsf_header.hpp"

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <span>

namespace igi::retail::audio {
namespace {

constexpr std::size_t retail_ilsf_header_size = 20U;
constexpr std::byte retail_ilsf_signature[] = {
    std::byte{ 'I' },
    std::byte{ 'L' },
    std::byte{ 'S' },
    std::byte{ 'F' },
};

std::uint16_t read_little_endian_uint16(std::span<const std::byte> input, std::size_t offset) noexcept
{
    const auto low_byte = static_cast<std::uint16_t>(std::to_integer<unsigned char>(input[offset]));
    const auto high_byte = static_cast<std::uint16_t>(std::to_integer<unsigned char>(input[offset + 1U]));
    return static_cast<std::uint16_t>(low_byte | static_cast<std::uint16_t>(high_byte << 8U));
}

std::uint32_t read_little_endian_uint32(std::span<const std::byte> input, std::size_t offset) noexcept
{
    const auto byte_zero = static_cast<std::uint32_t>(std::to_integer<unsigned char>(input[offset]));
    const auto byte_one = static_cast<std::uint32_t>(std::to_integer<unsigned char>(input[offset + 1U]));
    const auto byte_two = static_cast<std::uint32_t>(std::to_integer<unsigned char>(input[offset + 2U]));
    const auto byte_three = static_cast<std::uint32_t>(std::to_integer<unsigned char>(input[offset + 3U]));

    return byte_zero
        | (byte_one << 8U)
        | (byte_two << 16U)
        | (byte_three << 24U);
}

} // namespace

SoundIlsfHeaderReadResult Sound_ReadIlsfHeader(std::span<const std::byte> input) noexcept
{
    SoundIlsfHeaderReadResult result{};
    if (input.size() < retail_ilsf_header_size) {
        result.error = SoundIlsfHeaderError::header_too_short;
        return result;
    }

    for (std::size_t byte_index = 0U; byte_index < std::size(retail_ilsf_signature); ++byte_index) {
        if (input[byte_index] != retail_ilsf_signature[byte_index]) {
            result.error = SoundIlsfHeaderError::invalid_signature;
            return result;
        }
    }

    result.header.storage_format = read_little_endian_uint16(input, 4U);
    result.header.decoded_bits_per_sample = read_little_endian_uint16(input, 6U);
    result.header.channel_count = read_little_endian_uint16(input, 8U);
    result.header.reserved_value = read_little_endian_uint16(input, 10U);
    result.header.sample_rate_hertz = read_little_endian_uint32(input, 12U);
    result.header.frame_count = read_little_endian_uint32(input, 16U);
    return result;
}

} // namespace igi::retail::audio
