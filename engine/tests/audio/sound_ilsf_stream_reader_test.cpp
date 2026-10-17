#include "igi/retail/audio/sound_ilsf_stream_reader.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <streambuf>
#include <vector>

namespace {

using igi::retail::audio::SoundAdpcmState;
using igi::retail::audio::SoundIlsfStreamError;
using igi::retail::audio::SoundIlsfStreamReader;

class NonSeekableStringBuffer final : public std::stringbuf {
public:
    explicit NonSeekableStringBuffer(std::string content)
        : std::stringbuf(std::move(content), std::ios::in | std::ios::binary)
    {
    }

protected:
    pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode) override
    {
        return pos_type(off_type(-1));
    }

    pos_type seekpos(pos_type, std::ios_base::openmode) override
    {
        return pos_type(off_type(-1));
    }
};

void require(bool condition, std::string_view message)
{
    if (!condition) {
        throw std::runtime_error(std::string(message));
    }
}

void write_u16(std::vector<char>& bytes, std::size_t offset, std::uint16_t value)
{
    bytes[offset] = static_cast<char>(value & 0xFFU);
    bytes[offset + 1U] = static_cast<char>((value >> 8U) & 0xFFU);
}

void write_u32(std::vector<char>& bytes, std::size_t offset, std::uint32_t value)
{
    for (std::size_t byte_index = 0U; byte_index < sizeof(value); ++byte_index) {
        bytes[offset + byte_index] = static_cast<char>((value >> (byte_index * 8U)) & 0xFFU);
    }
}

[[nodiscard]] std::string make_ilsf(
    std::uint16_t storage_format,
    std::uint16_t bits_per_sample,
    std::uint16_t channel_count,
    std::uint32_t sample_rate_hertz,
    std::uint32_t frame_count,
    std::span<const std::uint8_t> encoded_payload)
{
    std::vector<char> file_bytes(20U + encoded_payload.size());
    file_bytes[0U] = 'I';
    file_bytes[1U] = 'L';
    file_bytes[2U] = 'S';
    file_bytes[3U] = 'F';
    write_u16(file_bytes, 4U, storage_format);
    write_u16(file_bytes, 6U, bits_per_sample);
    write_u16(file_bytes, 8U, channel_count);
    write_u16(file_bytes, 10U, 0U);
    write_u32(file_bytes, 12U, sample_rate_hertz);
    write_u32(file_bytes, 16U, frame_count);
    for (std::size_t byte_index = 0U; byte_index < encoded_payload.size(); ++byte_index) {
        file_bytes[20U + byte_index] = static_cast<char>(encoded_payload[byte_index]);
    }
    return std::string(file_bytes.begin(), file_bytes.end());
}

[[nodiscard]] std::vector<std::int16_t> read_all_frames(SoundIlsfStreamReader& reader, std::size_t chunk_frames)
{
    const std::size_t channel_count = static_cast<std::size_t>(reader.header().channel_count);
    std::vector<std::int16_t> output_samples;
    std::vector<std::int16_t> frame_buffer(chunk_frames * channel_count);
    while (!reader.end_of_sound()) {
        const std::size_t frames_read = reader.Sound_ReadIlsfFrames(frame_buffer);
        require(frames_read > 0U, "reader made no progress before the declared end of sound");
        output_samples.insert(
            output_samples.end(),
            frame_buffer.begin(),
            frame_buffer.begin() + static_cast<std::ptrdiff_t>(frames_read * channel_count));
    }
    require(reader.Sound_ReadIlsfFrames(frame_buffer) == 0U, "reader returns zero after the declared frame count");
    return output_samples;
}

void test_streamed_mono_adpcm_preserves_native_replayed_byte_behavior()
{
    constexpr std::array<std::uint8_t, 5U> encoded_payload = { 0x71U, 0x8FU, 0x24U, 0xC3U, 0x59U };
    const std::string file_bytes = make_ilsf(2U, 16U, 1U, 22050U, 9U, encoded_payload);
    std::istringstream input_stream(file_bytes, std::ios::binary);
    SoundIlsfStreamReader reader(input_stream);

    SoundAdpcmState contiguous_state{};
    igi::retail::audio::Sound_InitializeAdpcmState_00496420(contiguous_state, 1);
    std::array<std::int16_t, 9U> contiguous_samples{};
    const auto decode_result = igi::retail::audio::Sound_DecodeAdpcmFrames_00496280(
        encoded_payload,
        contiguous_samples,
        9U,
        contiguous_state);
    require(decode_result.succeeded(), "the contiguous retail decoder accepts the mono fixture");

    const std::vector<std::int16_t> streamed_samples = read_all_frames(reader, 3U);
    require(streamed_samples.size() == contiguous_samples.size(), "all declared mono frames are returned");
    require(std::equal(streamed_samples.begin(), streamed_samples.end(), contiguous_samples.begin()),
        "frame streaming matches contiguous retail ADPCM output across odd nibble boundaries");
    require(reader.frames_read() == 9U, "the frame cursor advances by frames rather than encoded bytes");
}

void test_streamed_stereo_pcm_reads_little_endian_frames_and_restarts()
{
    constexpr std::array<std::uint8_t, 12U> encoded_payload = {
        0x01U, 0x00U, 0xFFU, 0xFFU, 0xE8U, 0x03U,
        0x18U, 0xFCU, 0x00U, 0x80U, 0xFFU, 0x7FU,
    };
    const std::string file_bytes = make_ilsf(0U, 16U, 2U, 11025U, 3U, encoded_payload);
    std::istringstream input_stream(file_bytes, std::ios::binary);
    SoundIlsfStreamReader reader(input_stream);
    const std::vector<std::int16_t> first_pass = read_all_frames(reader, 2U);
    constexpr std::array<std::int16_t, 6U> expected_samples = { 1, -1, 1000, -1000, -32768, 32767 };
    require(std::equal(first_pass.begin(), first_pass.end(), expected_samples.begin()),
        "streamed PCM is decoded as little-endian signed 16-bit channel-interleaved samples");
    require(reader.can_restart(), "a seekable source reports restart support");
    reader.Sound_RestartIlsfStream();
    require(read_all_frames(reader, 1U) == first_pass, "PCM restart reproduces the same frames");
}

void test_resident_adpcm_storage_code_uses_the_same_sample_decoder()
{
    constexpr std::array<std::uint8_t, 2U> encoded_payload = { 0x79U, 0x2FU };
    const std::string file_bytes = make_ilsf(3U, 16U, 2U, 22050U, 2U, encoded_payload);
    std::istringstream input_stream(file_bytes, std::ios::binary);
    SoundIlsfStreamReader reader(input_stream);
    const std::vector<std::int16_t> output_samples = read_all_frames(reader, 1U);
    require(output_samples.size() == 4U, "resident ADPCM produces the declared two stereo frames");
    require(output_samples[0U] == 11 && output_samples[1U] == -1,
        "resident ADPCM retains the retail per-channel predictor state");
}

void test_nonseekable_input_remains_readable_and_rejects_restart()
{
    constexpr std::array<std::uint8_t, 4U> encoded_payload = { 0x01U, 0x00U, 0xFFU, 0x7FU };
    NonSeekableStringBuffer input_buffer(make_ilsf(0U, 16U, 1U, 22050U, 2U, encoded_payload));
    std::istream input_stream(&input_buffer);
    SoundIlsfStreamReader reader(input_stream);
    require(!reader.can_restart(), "nonseekable input is reported as nonrestartable");

    std::array<std::int16_t, 2U> output_samples{};
    require(reader.Sound_ReadIlsfFrames(output_samples) == 2U,
        "header position probing does not break subsequent reads on a nonseekable input");
    bool rejected_restart = false;
    try {
        reader.Sound_RestartIlsfStream();
    } catch (const SoundIlsfStreamError&) {
        rejected_restart = true;
    }
    require(rejected_restart, "restart rejects a nonseekable source with an explicit error");
}

void test_pcm_8_bit_is_unsigned_and_truncated_payload_is_rejected()
{
    constexpr std::array<std::uint8_t, 3U> pcm_payload = { 0x00U, 0x80U, 0xFFU };
    const std::string file_bytes = make_ilsf(1U, 8U, 1U, 44100U, 3U, pcm_payload);
    std::istringstream input_stream(file_bytes, std::ios::binary);
    SoundIlsfStreamReader reader(input_stream);
    const std::vector<std::int16_t> samples = read_all_frames(reader, 2U);
    constexpr std::array<std::int16_t, 3U> expected_samples = { -32768, 0, 32512 };
    require(std::equal(samples.begin(), samples.end(), expected_samples.begin()),
        "8-bit PCM uses unsigned silence at 0x80");

    const std::string truncated_file = make_ilsf(0U, 16U, 1U, 22050U, 2U,
        std::span<const std::uint8_t>(pcm_payload).first(1U));
    std::istringstream truncated_stream(truncated_file, std::ios::binary);
    SoundIlsfStreamReader truncated_reader(truncated_stream);
    std::array<std::int16_t, 2U> output_samples{};
    bool rejected_truncated_payload = false;
    try {
        static_cast<void>(truncated_reader.Sound_ReadIlsfFrames(output_samples));
    } catch (const SoundIlsfStreamError&) {
        rejected_truncated_payload = true;
    }
    require(rejected_truncated_payload, "truncated PCM is rejected before a frame is reported complete");
}

void test_level_one_sound_folder(const std::filesystem::path& sound_directory)
{
    std::size_t file_count = 0U;
    std::uint64_t frame_total = 0U;
    std::size_t streamed_pcm_count = 0U;
    std::size_t streamed_adpcm_count = 0U;
    std::vector<std::int16_t> frame_buffer(4096U);

    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(sound_directory)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".wav") {
            continue;
        }
        std::ifstream input_stream(entry.path(), std::ios::binary);
        if (!input_stream) {
            throw std::runtime_error("Could not open Level 1 sound: " + entry.path().string());
        }
        SoundIlsfStreamReader reader(input_stream);
        if (reader.header().storage_format == 0U) {
            ++streamed_pcm_count;
        } else if (reader.header().storage_format == 2U) {
            ++streamed_adpcm_count;
        }

        std::uint64_t file_frame_count = 0U;
        for (;;) {
            const std::size_t frames_read = reader.Sound_ReadIlsfFrames(frame_buffer);
            if (frames_read == 0U) {
                break;
            }
            file_frame_count += static_cast<std::uint64_t>(frames_read);
        }
        require(file_frame_count == reader.header().frame_count,
            "Level 1 sound ends at its ILSF-declared frame count");
        frame_total += file_frame_count;
        ++file_count;
    }

    require(file_count > 0U, "Level 1 SOUNDS directory contains ILSF files");
    std::cout << "Level 1 ILSF streamed-reader corpus passed: files=" << file_count
              << "; frames=" << frame_total
              << "; streamed PCM=" << streamed_pcm_count
              << "; streamed ADPCM=" << streamed_adpcm_count << '\n';
}

} // namespace

int main(int argument_count, char** arguments)
{
    try {
        test_streamed_mono_adpcm_preserves_native_replayed_byte_behavior();
        test_streamed_stereo_pcm_reads_little_endian_frames_and_restarts();
        test_resident_adpcm_storage_code_uses_the_same_sample_decoder();
        test_nonseekable_input_remains_readable_and_rejects_restart();
        test_pcm_8_bit_is_unsigned_and_truncated_payload_is_rejected();
        if (argument_count == 2) {
            test_level_one_sound_folder(arguments[1]);
        } else if (argument_count != 1) {
            throw std::runtime_error("Expected zero arguments or one Level 1 SOUNDS directory path.");
        }
        std::cout << "All retail ILSF streaming tests passed.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Retail ILSF stream test failed: " << exception.what() << '\n';
        return 1;
    }
}
