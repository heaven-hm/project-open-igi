#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace igi::formats::sound
{

enum class IlsfStorageFormat : std::uint16_t
{
    streamed_pcm = 0,
    resident_pcm = 1,
    streamed_adpcm = 2,
    resident_adpcm = 3,
};

struct IlsfSoundData
{
    IlsfStorageFormat storage_format = IlsfStorageFormat::resident_pcm;
    std::uint16_t bits_per_sample = 16;
    std::uint16_t channel_count = 1;
    std::uint16_t reserved_value = 0;
    std::uint32_t sample_rate = 0;
    std::uint32_t frame_count = 0;
    std::vector<std::uint8_t> encoded_payload;

    [[nodiscard]] bool IsAdpcm() const noexcept;
    [[nodiscard]] bool IsStreamed() const noexcept;
    [[nodiscard]] std::uint64_t DecodedSampleCount() const noexcept;
};

// Original IGI.EXE sound-loader native at VA 0x00495F70 recognizes the ILSF signature.
[[nodiscard]] bool SoundSystem_IsIlsfFile(std::span<const std::uint8_t> data) noexcept;

// Original IGI.EXE sound-loader native at VA 0x00495F70 parses the 20-byte ILSF header and payload.
[[nodiscard]] IlsfSoundData SoundSystem_ParseIlsfFile(std::span<const std::uint8_t> data);

// Original IGI.EXE sound-loader/ADPCM natives at VAs 0x00495F70, 0x00496420, and 0x00496280.
[[nodiscard]] std::vector<std::int16_t> SoundSystem_DecodeIlsfFileToPcm16(const IlsfSoundData& sound);

} // namespace igi::formats::sound
