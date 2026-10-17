#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace igi::retail::audio {

enum class SoundIlsfHeaderError : std::uint8_t {
    none,
    header_too_short,
    invalid_signature,
};

struct SoundIlsfHeader {
    std::uint16_t storage_format = 0U;
    std::uint16_t decoded_bits_per_sample = 0U;
    std::uint16_t channel_count = 0U;
    std::uint16_t reserved_value = 0U;
    std::uint32_t sample_rate_hertz = 0U;
    std::uint32_t frame_count = 0U;
};

struct SoundIlsfHeaderReadResult {
    SoundIlsfHeaderError error = SoundIlsfHeaderError::none;
    SoundIlsfHeader header{};

    [[nodiscard]] bool succeeded() const noexcept
    {
        return error == SoundIlsfHeaderError::none;
    }
};

// Helper for the retail sound loader at VA 0x00495F70; this is not a separate retail entrypoint.
[[nodiscard]] SoundIlsfHeaderReadResult Sound_ReadIlsfHeader(std::span<const std::byte> input) noexcept;

} // namespace igi::retail::audio
