#include "igi/formats/ilsf_sound.hpp"

#include "igi/exceptions.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <string>

namespace igi::formats::sound
{
namespace
{

constexpr std::size_t ilsf_header_size = 20;
constexpr std::uint16_t ilsf_adpcm_bits_per_sample = 16;
constexpr std::uint16_t maximum_ilsf_channels = 2;
constexpr std::uint32_t maximum_addressable_frame_count = 0x7fffffffU;
constexpr std::array<std::int8_t, 16> adpcm_step_index_adjustments = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};
constexpr std::array<std::int16_t, 89> adpcm_step_sizes = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767,
};

std::uint16_t read_little_endian_u16(std::span<const std::uint8_t> data, std::size_t offset)
{
    return static_cast<std::uint16_t>(data[offset]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(data[offset + 1]) << 8U);
}

std::uint32_t read_little_endian_u32(std::span<const std::uint8_t> data, std::size_t offset)
{
    return static_cast<std::uint32_t>(data[offset]) |
           (static_cast<std::uint32_t>(data[offset + 1]) << 8U) |
           (static_cast<std::uint32_t>(data[offset + 2]) << 16U) |
           (static_cast<std::uint32_t>(data[offset + 3]) << 24U);
}

std::uint64_t SoundSystem_GetEncodedPayloadByteCount(const IlsfSoundData& sound)
{
    const std::uint64_t interleaved_sample_count =
        static_cast<std::uint64_t>(sound.frame_count) * sound.channel_count;
    if (sound.IsAdpcm())
    {
        return (interleaved_sample_count + 1U) / 2U;
    }

    return (interleaved_sample_count * sound.bits_per_sample) / 8U;
}

std::vector<std::int16_t> SoundSystem_DecodeIlsfAdpcm(
    const IlsfSoundData& sound,
    std::size_t sample_count)
{
    // Retail FUN_00496420 initializes one predictor and step index per channel before FUN_00496280.
    std::array<std::int32_t, maximum_ilsf_channels> predictors{};
    std::array<std::int32_t, maximum_ilsf_channels> step_indices{};
    std::vector<std::int16_t> decoded_samples(sample_count);
    std::size_t interleaved_sample_index = 0;

    for (std::uint32_t frame_index = 0; frame_index < sound.frame_count; ++frame_index)
    {
        for (std::uint16_t channel_index = 0; channel_index < sound.channel_count; ++channel_index)
        {
            const std::size_t byte_index = interleaved_sample_index / 2U;
            const bool take_high_nibble = interleaved_sample_index % 2U == 0U;
            const std::uint8_t encoded_byte = sound.encoded_payload[byte_index];
            const std::uint8_t encoded_nibble = take_high_nibble
                ? static_cast<std::uint8_t>(encoded_byte >> 4U)
                : static_cast<std::uint8_t>(encoded_byte & 0x0fU);
            const std::int32_t previous_step_index = step_indices[channel_index];
            const std::int32_t current_step_size = adpcm_step_sizes[static_cast<std::size_t>(previous_step_index)];

            // Retail FUN_00496280 updates the index but computes this sample with the previous step size.
            step_indices[channel_index] = std::clamp(
                previous_step_index + adpcm_step_index_adjustments[encoded_nibble], 0, 88);

            std::int32_t difference = current_step_size >> 3;
            if ((encoded_nibble & 0x04U) != 0U)
            {
                difference += current_step_size;
            }
            if ((encoded_nibble & 0x02U) != 0U)
            {
                difference += current_step_size >> 1;
            }
            if ((encoded_nibble & 0x01U) != 0U)
            {
                difference += current_step_size >> 2;
            }
            if ((encoded_nibble & 0x08U) != 0U)
            {
                difference = -difference;
            }

            predictors[channel_index] = std::clamp(
                predictors[channel_index] + difference,
                static_cast<std::int32_t>(std::numeric_limits<std::int16_t>::min()),
                static_cast<std::int32_t>(std::numeric_limits<std::int16_t>::max()));
            decoded_samples[interleaved_sample_index] =
                static_cast<std::int16_t>(predictors[channel_index]);
            ++interleaved_sample_index;
        }
    }

    return decoded_samples;
}

} // namespace

bool IlsfSoundData::IsAdpcm() const noexcept
{
    return storage_format == IlsfStorageFormat::streamed_adpcm ||
           storage_format == IlsfStorageFormat::resident_adpcm;
}

bool IlsfSoundData::IsStreamed() const noexcept
{
    return storage_format == IlsfStorageFormat::streamed_pcm ||
           storage_format == IlsfStorageFormat::streamed_adpcm;
}

std::uint64_t IlsfSoundData::DecodedSampleCount() const noexcept
{
    return static_cast<std::uint64_t>(frame_count) * channel_count;
}

// Original IGI.EXE sound loader native at VA 0x00495F70 recognizes the ILSF file signature.
bool SoundSystem_IsIlsfFile(std::span<const std::uint8_t> data) noexcept
{
    return data.size() >= ilsf_header_size &&
           data[0] == 'I' && data[1] == 'L' && data[2] == 'S' && data[3] == 'F';
}

// Original IGI.EXE sound loader native at VA 0x00495F70 validates and dispatches ILSF formats.
IlsfSoundData SoundSystem_ParseIlsfFile(std::span<const std::uint8_t> data)
{
    if (data.size() < ilsf_header_size)
    {
        throw logging::AudioException("ILSF sound is shorter than its 20-byte header");
    }
    if (!SoundSystem_IsIlsfFile(data))
    {
        throw logging::AudioException("ILSF sound header has invalid magic; expected ILSF");
    }

    const std::uint16_t raw_storage_format = read_little_endian_u16(data, 4);
    if (raw_storage_format > static_cast<std::uint16_t>(IlsfStorageFormat::resident_adpcm))
    {
        throw logging::AudioException("ILSF sound has unsupported storage format " +
                                      std::to_string(raw_storage_format));
    }

    IlsfSoundData sound;
    sound.storage_format = static_cast<IlsfStorageFormat>(raw_storage_format);
    sound.bits_per_sample = read_little_endian_u16(data, 6);
    sound.channel_count = read_little_endian_u16(data, 8);
    sound.reserved_value = read_little_endian_u16(data, 10);
    sound.sample_rate = read_little_endian_u32(data, 12);
    sound.frame_count = read_little_endian_u32(data, 16);

    if (sound.bits_per_sample != 8 && sound.bits_per_sample != 16)
    {
        throw logging::AudioException("ILSF sound supports only 8-bit or 16-bit samples");
    }
    if (sound.channel_count == 0 || sound.channel_count > maximum_ilsf_channels)
    {
        throw logging::AudioException("ILSF sound channel count must be one or two");
    }
    if (sound.sample_rate == 0)
    {
        throw logging::AudioException("ILSF sound sample rate must be positive");
    }
    if (sound.frame_count > maximum_addressable_frame_count)
    {
        throw logging::AudioException("ILSF sound frame count exceeds the retail addressable range");
    }
    if (sound.IsAdpcm() && sound.bits_per_sample != ilsf_adpcm_bits_per_sample)
    {
        throw logging::AudioException("ILSF ADPCM data must declare 16-bit decoded samples");
    }

    const std::uint64_t encoded_payload_byte_count = SoundSystem_GetEncodedPayloadByteCount(sound);
    const std::uint64_t available_payload_byte_count = data.size() - ilsf_header_size;
    if (encoded_payload_byte_count > available_payload_byte_count)
    {
        throw logging::AudioException("ILSF sound payload is shorter than its declared frame count");
    }
    if (encoded_payload_byte_count > std::numeric_limits<std::size_t>::max() ||
        encoded_payload_byte_count > static_cast<std::uint64_t>(std::numeric_limits<std::ptrdiff_t>::max()))
    {
        throw logging::AudioException("ILSF sound payload cannot fit in this process address space");
    }

    const std::size_t payload_byte_count = static_cast<std::size_t>(encoded_payload_byte_count);
    const auto payload_begin = data.begin() + static_cast<std::ptrdiff_t>(ilsf_header_size);
    sound.encoded_payload.assign(payload_begin, payload_begin + static_cast<std::ptrdiff_t>(payload_byte_count));
    return sound;
}

// Original IGI.EXE ADPCM natives at VAs 0x00496420 and 0x00496280 initialize and decode ILSF samples.
std::vector<std::int16_t> SoundSystem_DecodeIlsfFileToPcm16(const IlsfSoundData& sound)
{
    const std::uint64_t sample_count_as_u64 = sound.DecodedSampleCount();
    if (sample_count_as_u64 > std::numeric_limits<std::size_t>::max())
    {
        throw logging::AudioException("Decoded ILSF sound does not fit in this process address space");
    }

    const std::size_t sample_count = static_cast<std::size_t>(sample_count_as_u64);
    if (sound.IsAdpcm())
    {
        if (sound.bits_per_sample != ilsf_adpcm_bits_per_sample ||
            sound.channel_count == 0 || sound.channel_count > maximum_ilsf_channels)
        {
            throw logging::AudioException("ILSF ADPCM metadata is invalid for the retail decoder");
        }
        if (SoundSystem_GetEncodedPayloadByteCount(sound) > sound.encoded_payload.size())
        {
            throw logging::AudioException("ILSF ADPCM payload is truncated");
        }
        return SoundSystem_DecodeIlsfAdpcm(sound, sample_count);
    }

    if (sound.channel_count == 0 || sound.channel_count > maximum_ilsf_channels ||
        (sound.bits_per_sample != 8 && sound.bits_per_sample != 16))
    {
        throw logging::AudioException("ILSF PCM metadata is invalid");
    }
    if (SoundSystem_GetEncodedPayloadByteCount(sound) > sound.encoded_payload.size())
    {
        throw logging::AudioException("ILSF PCM payload is truncated");
    }

    std::vector<std::int16_t> decoded_samples(sample_count);
    if (sound.bits_per_sample == 8)
    {
        for (std::size_t sample_index = 0; sample_index < sample_count; ++sample_index)
        {
            const std::int32_t centered_sample =
                static_cast<std::int32_t>(sound.encoded_payload[sample_index]) - 0x80;
            decoded_samples[sample_index] = static_cast<std::int16_t>(centered_sample << 8);
        }
        return decoded_samples;
    }

    for (std::size_t sample_index = 0; sample_index < sample_count; ++sample_index)
    {
        const std::uint16_t unsigned_sample =
            static_cast<std::uint16_t>(sound.encoded_payload[sample_index * 2U]) |
            static_cast<std::uint16_t>(static_cast<std::uint16_t>(sound.encoded_payload[sample_index * 2U + 1U])
                                       << 8U);
        decoded_samples[sample_index] = static_cast<std::int16_t>(unsigned_sample);
    }
    return decoded_samples;
}

} // namespace igi::formats::sound
