#include "igi/retail/audio/sound_ilsf_stream_reader.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <string>
#include <vector>

namespace igi::retail::audio {
namespace {

constexpr std::size_t ilsf_header_size = 20U;
constexpr std::uint16_t streamed_adpcm_format = 2U;
constexpr std::uint16_t resident_adpcm_format = 3U;

[[nodiscard]] bool is_adpcm_format(std::uint16_t storage_format) noexcept
{
    return storage_format == streamed_adpcm_format || storage_format == resident_adpcm_format;
}

[[nodiscard]] std::size_t required_adpcm_bytes(std::size_t sample_count, bool has_pending_nibble)
{
    if (sample_count == 0U) {
        return 0U;
    }
    if (has_pending_nibble) {
        const std::size_t remaining_sample_count = sample_count - 1U;
        return 1U + remaining_sample_count / 2U + remaining_sample_count % 2U;
    }
    return sample_count / 2U + sample_count % 2U;
}

[[nodiscard]] std::uint16_t read_little_endian_uint16(const std::byte* bytes) noexcept
{
    const auto low_byte = static_cast<std::uint16_t>(std::to_integer<std::uint8_t>(bytes[0U]));
    const auto high_byte = static_cast<std::uint16_t>(std::to_integer<std::uint8_t>(bytes[1U]));
    return static_cast<std::uint16_t>(low_byte | static_cast<std::uint16_t>(high_byte << 8U));
}

void validate_header(const SoundIlsfHeader& header)
{
    if (header.storage_format > resident_adpcm_format) {
        throw SoundIlsfStreamError("ILSF storage format is outside the four retail loader branches.");
    }
    if (header.decoded_bits_per_sample != 8U && header.decoded_bits_per_sample != 16U) {
        throw SoundIlsfStreamError("ILSF decoded sample width is neither 8 nor 16 bits.");
    }
    if (header.channel_count == 0U || header.channel_count > 2U) {
        throw SoundIlsfStreamError("ILSF channel count is outside the retail decoder range of one to two.");
    }
    if (header.sample_rate_hertz == 0U) {
        throw SoundIlsfStreamError("ILSF sample rate is zero.");
    }
    if (is_adpcm_format(header.storage_format) && header.decoded_bits_per_sample != 16U) {
        throw SoundIlsfStreamError("Retail ILSF ADPCM decodes to signed 16-bit samples.");
    }
}

} // namespace

SoundIlsfStreamReader::SoundIlsfStreamReader(std::istream& input_stream)
    : input_stream_(&input_stream)
{
    std::array<std::byte, ilsf_header_size> header_bytes{};
    read_exact(header_bytes);

    const SoundIlsfHeaderReadResult header_result = Sound_ReadIlsfHeader(header_bytes);
    if (!header_result.succeeded()) {
        throw SoundIlsfStreamError(
            header_result.error == SoundIlsfHeaderError::header_too_short
                ? "ILSF stream ended inside its 20-byte header."
                : "ILSF stream does not begin with the retail ILSF signature.");
    }
    header_ = header_result.header;
    validate_header(header_);

    payload_start_ = input_stream_->rdbuf()->pubseekoff(
        0,
        std::ios_base::cur,
        std::ios_base::in);
    can_restart_ = payload_start_ != std::streampos(-1);
    if (is_adpcm_format(header_.storage_format)) {
        Sound_InitializeAdpcmState_00496420(
            adpcm_state_,
            static_cast<std::int16_t>(header_.channel_count));
    }
}

const SoundIlsfHeader& SoundIlsfStreamReader::header() const noexcept
{
    return header_;
}

std::uint64_t SoundIlsfStreamReader::frames_read() const noexcept
{
    return frames_read_;
}

bool SoundIlsfStreamReader::end_of_sound() const noexcept
{
    return frames_read_ >= header_.frame_count;
}

bool SoundIlsfStreamReader::can_restart() const noexcept
{
    return can_restart_;
}

std::size_t SoundIlsfStreamReader::Sound_ReadIlsfFrames(std::span<std::int16_t> output_samples)
{
    if (end_of_sound()) {
        return 0U;
    }

    const std::size_t channel_count = static_cast<std::size_t>(header_.channel_count);
    const std::size_t frame_capacity = output_samples.size() / channel_count;
    if (frame_capacity == 0U) {
        throw SoundIlsfStreamError("Output span cannot hold one complete ILSF audio frame.");
    }

    const std::uint64_t remaining_frames =
        static_cast<std::uint64_t>(header_.frame_count) - frames_read_;
    const std::size_t frames_to_read = static_cast<std::size_t>(
        std::min<std::uint64_t>(remaining_frames, static_cast<std::uint64_t>(frame_capacity)));
    if (frames_to_read > std::numeric_limits<std::size_t>::max() / channel_count) {
        throw SoundIlsfStreamError("Requested ILSF sample count overflows the host address range.");
    }
    const std::size_t sample_count = frames_to_read * channel_count;
    const std::span<std::int16_t> frame_output = output_samples.first(sample_count);

    if (is_adpcm_format(header_.storage_format)) {
        const bool has_pending_nibble = adpcm_state_.pending_low_nibble != 0U;
        const std::size_t encoded_byte_count = required_adpcm_bytes(sample_count, has_pending_nibble);
        const std::size_t new_encoded_byte_count = encoded_byte_count - (has_pending_nibble ? 1U : 0U);
        std::vector<std::uint8_t> encoded_bytes(encoded_byte_count);
        if (has_pending_nibble) {
            encoded_bytes[0U] = replayed_adpcm_byte_;
        }
        read_exact(std::span<std::uint8_t>(encoded_bytes).subspan(has_pending_nibble ? 1U : 0U));

        const SoundAdpcmDecodeResult decode_result = Sound_DecodeAdpcmFrames_00496280(
            encoded_bytes,
            frame_output,
            frames_to_read,
            adpcm_state_);
        if (!decode_result.succeeded() || decode_result.decoded_samples_written != sample_count) {
            throw SoundIlsfStreamError("Retail ILSF ADPCM decoder rejected a bounded frame block.");
        }
        if (new_encoded_byte_count > 0U) {
            replayed_adpcm_byte_ = encoded_bytes.back();
        }
    } else {
        const std::size_t bytes_per_sample = static_cast<std::size_t>(header_.decoded_bits_per_sample / 8U);
        if (sample_count > std::numeric_limits<std::size_t>::max() / bytes_per_sample) {
            throw SoundIlsfStreamError("Requested ILSF PCM byte count overflows the host address range.");
        }
        const std::size_t encoded_byte_count = sample_count * bytes_per_sample;
        std::vector<std::byte> encoded_bytes(encoded_byte_count);
        read_exact(encoded_bytes);

        if (header_.decoded_bits_per_sample == 16U) {
            for (std::size_t sample_index = 0U; sample_index < sample_count; ++sample_index) {
                const std::uint16_t sample_bits = read_little_endian_uint16(
                    encoded_bytes.data() + sample_index * sizeof(std::uint16_t));
                frame_output[sample_index] = static_cast<std::int16_t>(sample_bits);
            }
        } else {
            for (std::size_t sample_index = 0U; sample_index < sample_count; ++sample_index) {
                const std::int32_t unsigned_sample =
                    static_cast<std::int32_t>(std::to_integer<std::uint8_t>(encoded_bytes[sample_index]));
                frame_output[sample_index] = static_cast<std::int16_t>((unsigned_sample - 128) * 256);
            }
        }
    }

    frames_read_ += static_cast<std::uint64_t>(frames_to_read);
    return frames_to_read;
}

void SoundIlsfStreamReader::Sound_RestartIlsfStream()
{
    if (!can_restart_) {
        throw SoundIlsfStreamError("ILSF stream cannot restart because its source is not seekable.");
    }

    input_stream_->clear();
    input_stream_->seekg(payload_start_);
    if (!*input_stream_) {
        throw SoundIlsfStreamError("Could not seek to the beginning of the ILSF payload.");
    }

    frames_read_ = 0U;
    replayed_adpcm_byte_ = 0U;
    if (is_adpcm_format(header_.storage_format)) {
        Sound_InitializeAdpcmState_00496420(
            adpcm_state_,
            static_cast<std::int16_t>(header_.channel_count));
    }
}

void SoundIlsfStreamReader::read_exact(std::span<std::byte> destination)
{
    if (destination.empty()) {
        return;
    }
    input_stream_->read(
        reinterpret_cast<char*>(destination.data()),
        static_cast<std::streamsize>(destination.size()));
    if (input_stream_->gcount() != static_cast<std::streamsize>(destination.size())) {
        throw SoundIlsfStreamError("ILSF PCM payload ended before the declared frame count.");
    }
}

void SoundIlsfStreamReader::read_exact(std::span<std::uint8_t> destination)
{
    if (destination.empty()) {
        return;
    }
    input_stream_->read(
        reinterpret_cast<char*>(destination.data()),
        static_cast<std::streamsize>(destination.size()));
    if (input_stream_->gcount() != static_cast<std::streamsize>(destination.size())) {
        throw SoundIlsfStreamError("ILSF ADPCM payload ended before the declared frame count.");
    }
}

} // namespace igi::retail::audio
