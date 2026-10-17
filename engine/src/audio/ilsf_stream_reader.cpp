#include "igi/audio/ilsf_stream_reader.hpp"

#include "igi/exceptions.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <ios>
#include <limits>
#include <string>

namespace igi::audio
{
namespace
{

constexpr std::size_t ilsf_header_size = 20;

std::size_t SoundSystem_GetFramePayloadByteCount(
    const formats::sound::IlsfSoundData& header,
    std::size_t frame_count,
    bool has_pending_adpcm_nibble)
{
    if (header.IsAdpcm())
    {
        const std::size_t sample_count = frame_count * header.channel_count;
        const std::size_t nibble_count = sample_count - (has_pending_adpcm_nibble ? 1U : 0U);
        return (nibble_count + 1U) / 2U;
    }

    return frame_count * header.channel_count * (header.bits_per_sample / 8U);
}

void SoundSystem_ReadExactly(
    std::istream& source,
    std::span<std::uint8_t> destination,
    std::uint64_t frames_already_read,
    std::uint32_t total_frame_count)
{
    if (destination.empty())
    {
        return;
    }
    if (static_cast<std::uintmax_t>(destination.size()) >
        static_cast<std::uintmax_t>(std::numeric_limits<std::streamsize>::max()))
    {
        throw logging::AudioException("ILSF stream read exceeds the platform stream-size range");
    }

    try
    {
        source.read(reinterpret_cast<char*>(destination.data()),
                    static_cast<std::streamsize>(destination.size()));
    }
    catch (const std::ios_base::failure& exception)
    {
        throw logging::AudioException(
            "ILSF stream ended before frame " + std::to_string(frames_already_read) + " of " +
            std::to_string(total_frame_count) + ": " + exception.what());
    }

    if (source.gcount() != static_cast<std::streamsize>(destination.size()))
    {
        throw logging::AudioException(
            "ILSF stream ended after " + std::to_string(frames_already_read) + " of " +
            std::to_string(total_frame_count) + " declared frames");
    }
}

} // namespace

IlsfStreamReader::IlsfStreamReader(
    std::istream& source,
    formats::sound::IlsfSoundData header,
    std::optional<std::streampos> payload_start)
    : source_(source),
      header_(std::move(header)),
      payload_start_(payload_start)
{
    if (header_.IsAdpcm())
    {
        adpcm_state_ = formats::sound::SoundSystem_InitializeIlsfAdpcmDecoder(header_.channel_count);
    }
}

IlsfStreamReader IlsfStreamReader::Open(std::istream& source)
{
    if (source.rdbuf() == nullptr)
    {
        throw logging::AudioException("Cannot open an ILSF reader over a stream without a buffer");
    }

    const std::streampos invalid_position(std::streamoff(-1));
    const std::streampos source_start = source.rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::in);
    std::optional<std::streampos> payload_start;
    if (source_start != invalid_position)
    {
        payload_start = source_start + static_cast<std::streamoff>(ilsf_header_size);
    }

    std::array<std::uint8_t, ilsf_header_size> encoded_header{};
    try
    {
        source.read(reinterpret_cast<char*>(encoded_header.data()),
                    static_cast<std::streamsize>(encoded_header.size()));
    }
    catch (const std::ios_base::failure& exception)
    {
        throw logging::AudioException(
            "ILSF sound needs a complete 20-byte header: " + std::string(exception.what()));
    }
    if (source.gcount() != static_cast<std::streamsize>(encoded_header.size()))
    {
        throw logging::AudioException("ILSF sound needs a complete 20-byte header");
    }

    formats::sound::IlsfSoundData header = formats::sound::SoundSystem_ParseIlsfHeader(encoded_header);
    return IlsfStreamReader(source, std::move(header), payload_start);
}

const formats::sound::IlsfSoundData& IlsfStreamReader::Header() const noexcept
{
    return header_;
}

std::uint64_t IlsfStreamReader::FramesRead() const noexcept
{
    return frames_read_;
}

bool IlsfStreamReader::EndOfSound() const noexcept
{
    return frames_read_ >= header_.frame_count;
}

bool IlsfStreamReader::CanRestart() const noexcept
{
    return payload_start_.has_value();
}

std::size_t IlsfStreamReader::ReadFrames(std::span<std::int16_t> destination)
{
    const std::size_t channel_count = header_.channel_count;
    const std::size_t frame_capacity = destination.size() / channel_count;
    if (frame_capacity == 0)
    {
        throw logging::AudioException("ILSF destination cannot hold one complete audio frame");
    }

    const std::uint64_t remaining_frame_count = header_.frame_count - frames_read_;
    const std::size_t frames_to_read = static_cast<std::size_t>(
        std::min<std::uint64_t>(frame_capacity, remaining_frame_count));
    if (frames_to_read == 0)
    {
        return 0;
    }

    const bool has_pending_adpcm_nibble = header_.IsAdpcm() && adpcm_state_.has_pending_nibble;
    const std::size_t encoded_byte_count = SoundSystem_GetFramePayloadByteCount(
        header_, frames_to_read, has_pending_adpcm_nibble);
    encoded_scratch_.resize(encoded_byte_count);
    SoundSystem_ReadExactly(source_, encoded_scratch_, frames_read_, header_.frame_count);

    const std::size_t decoded_sample_count = frames_to_read * channel_count;
    std::span<std::int16_t> output = destination.first(decoded_sample_count);
    if (header_.IsAdpcm())
    {
        const std::size_t consumed_byte_count = formats::sound::SoundSystem_DecodeIlsfAdpcmFrames(
            adpcm_state_, encoded_scratch_, output, frames_to_read);
        if (consumed_byte_count != encoded_byte_count)
        {
            throw logging::AudioException("Retail ILSF ADPCM stream consumed an unexpected byte count");
        }
    }
    else if (header_.bits_per_sample == 16)
    {
        for (std::size_t sample_index = 0; sample_index < decoded_sample_count; ++sample_index)
        {
            const std::uint16_t unsigned_sample =
                static_cast<std::uint16_t>(encoded_scratch_[sample_index * 2U]) |
                static_cast<std::uint16_t>(
                    static_cast<std::uint16_t>(encoded_scratch_[sample_index * 2U + 1U]) << 8U);
            output[sample_index] = static_cast<std::int16_t>(unsigned_sample);
        }
    }
    else
    {
        for (std::size_t sample_index = 0; sample_index < decoded_sample_count; ++sample_index)
        {
            const std::int32_t centered_sample =
                static_cast<std::int32_t>(encoded_scratch_[sample_index]) - 0x80;
            output[sample_index] = static_cast<std::int16_t>(centered_sample << 8);
        }
    }

    frames_read_ += frames_to_read;
    return frames_to_read;
}

void IlsfStreamReader::Restart()
{
    if (!payload_start_.has_value())
    {
        throw logging::AudioException("Cannot restart an ILSF stream that does not support seeking");
    }

    source_.clear();
    const std::streampos result = source_.rdbuf()->pubseekpos(*payload_start_, std::ios_base::in);
    const std::streampos invalid_position(std::streamoff(-1));
    if (result == invalid_position)
    {
        throw logging::AudioException("Could not seek to the start of the ILSF sound payload");
    }

    source_.clear();
    frames_read_ = 0;
    if (header_.IsAdpcm())
    {
        adpcm_state_ = formats::sound::SoundSystem_InitializeIlsfAdpcmDecoder(header_.channel_count);
    }
}

} // namespace igi::audio
