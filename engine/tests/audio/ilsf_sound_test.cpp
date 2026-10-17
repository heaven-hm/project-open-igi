#include "igi/audio/ilsf_stream_reader.hpp"
#include "igi/exceptions.hpp"
#include "igi/formats/ilsf_sound.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

using igi::audio::IlsfStreamReader;
using igi::formats::sound::IlsfAdpcmDecoderState;
using igi::formats::sound::IlsfSoundData;
using igi::formats::sound::IlsfStorageFormat;
using igi::formats::sound::SoundSystem_DecodeIlsfFileToPcm16;
using igi::formats::sound::SoundSystem_IsIlsfFile;
using igi::formats::sound::SoundSystem_DecodeIlsfAdpcmFrames;
using igi::formats::sound::SoundSystem_InitializeIlsfAdpcmDecoder;
using igi::formats::sound::SoundSystem_ParseIlsfFile;

void require(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void write_little_endian_u16(std::vector<std::uint8_t>& data, std::size_t offset, std::uint16_t value)
{
    data[offset] = static_cast<std::uint8_t>(value & 0xffU);
    data[offset + 1] = static_cast<std::uint8_t>(value >> 8U);
}

void write_little_endian_u32(std::vector<std::uint8_t>& data, std::size_t offset, std::uint32_t value)
{
    data[offset] = static_cast<std::uint8_t>(value & 0xffU);
    data[offset + 1] = static_cast<std::uint8_t>((value >> 8U) & 0xffU);
    data[offset + 2] = static_cast<std::uint8_t>((value >> 16U) & 0xffU);
    data[offset + 3] = static_cast<std::uint8_t>(value >> 24U);
}

std::vector<std::uint8_t> make_ilsf(
    IlsfStorageFormat storage_format,
    std::uint16_t bits_per_sample,
    std::uint16_t channel_count,
    std::uint32_t sample_rate,
    std::uint32_t frame_count,
    std::vector<std::uint8_t> payload)
{
    std::vector<std::uint8_t> data(20U + payload.size(), 0);
    data[0] = 'I';
    data[1] = 'L';
    data[2] = 'S';
    data[3] = 'F';
    write_little_endian_u16(data, 4, static_cast<std::uint16_t>(storage_format));
    write_little_endian_u16(data, 6, bits_per_sample);
    write_little_endian_u16(data, 8, channel_count);
    write_little_endian_u32(data, 12, sample_rate);
    write_little_endian_u32(data, 16, frame_count);
    std::copy(payload.begin(), payload.end(), data.begin() + 20);
    return data;
}

template <typename Callback>
void require_audio_exception(Callback&& callback, const std::string& message)
{
    bool exception_was_thrown = false;
    try
    {
        callback();
    }
    catch (const igi::logging::AudioException&)
    {
        exception_was_thrown = true;
    }
    require(exception_was_thrown, message);
}

std::vector<std::uint8_t> read_file(const std::filesystem::path& file_path)
{
    std::ifstream file(file_path, std::ios::binary);
    require(file.is_open(), "Could not open retail sound file: " + file_path.string());
    return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(file), {});
}

void test_all_storage_formats()
{
    const std::array<IlsfStorageFormat, 4> storage_formats = {
        IlsfStorageFormat::streamed_pcm,
        IlsfStorageFormat::resident_pcm,
        IlsfStorageFormat::streamed_adpcm,
        IlsfStorageFormat::resident_adpcm,
    };

    for (const IlsfStorageFormat storage_format : storage_formats)
    {
        const bool is_adpcm = storage_format == IlsfStorageFormat::streamed_adpcm ||
                              storage_format == IlsfStorageFormat::resident_adpcm;
        const std::vector<std::uint8_t> payload = is_adpcm
            ? std::vector<std::uint8_t>{0x70U}
            : std::vector<std::uint8_t>{0x00U, 0x00U};
        const std::vector<std::uint8_t> bytes = make_ilsf(storage_format, 16, 1, 22050, 1, payload);
        const IlsfSoundData sound = SoundSystem_ParseIlsfFile(bytes);
        require(sound.storage_format == storage_format, "ILSF storage format code was not preserved");
        require(sound.IsAdpcm() == is_adpcm, "ILSF PCM/ADPCM classification is incorrect");
        require(sound.IsStreamed() == (storage_format == IlsfStorageFormat::streamed_pcm ||
                                       storage_format == IlsfStorageFormat::streamed_adpcm),
                "ILSF streamed/resident classification is incorrect");
    }
}

void test_pcm_decoding()
{
    const std::vector<std::uint8_t> pcm8_bytes = make_ilsf(
        IlsfStorageFormat::resident_pcm, 8, 1, 22050, 3, {0x00U, 0x80U, 0xffU});
    const std::vector<std::int16_t> pcm8_samples =
        SoundSystem_DecodeIlsfFileToPcm16(SoundSystem_ParseIlsfFile(pcm8_bytes));
    require(pcm8_samples == std::vector<std::int16_t>{-32768, 0, 32512},
            "Retail unsigned 8-bit PCM must be centered and expanded to signed 16-bit samples");

    const std::vector<std::uint8_t> pcm16_bytes = make_ilsf(
        IlsfStorageFormat::resident_pcm, 16, 1, 22050, 2, {0x34U, 0x12U, 0x00U, 0x80U});
    const std::vector<std::int16_t> pcm16_samples =
        SoundSystem_DecodeIlsfFileToPcm16(SoundSystem_ParseIlsfFile(pcm16_bytes));
    require(pcm16_samples == std::vector<std::int16_t>{0x1234, -32768},
            "Retail 16-bit PCM must preserve signed little-endian samples");
}

void test_adpcm_decoding()
{
    const std::vector<std::uint8_t> mono_bytes = make_ilsf(
        IlsfStorageFormat::resident_adpcm, 16, 1, 22050, 2, {0x7fU});
    const std::vector<std::int16_t> mono_samples =
        SoundSystem_DecodeIlsfFileToPcm16(SoundSystem_ParseIlsfFile(mono_bytes));
    require(mono_samples == std::vector<std::int16_t>{11, -19},
            "Retail ADPCM must decode high nibble first using the previous step index");

    const std::vector<std::uint8_t> stereo_bytes = make_ilsf(
        IlsfStorageFormat::streamed_adpcm, 16, 2, 22050, 1, {0x7fU});
    const std::vector<std::int16_t> stereo_samples =
        SoundSystem_DecodeIlsfFileToPcm16(SoundSystem_ParseIlsfFile(stereo_bytes));
    require(stereo_samples == std::vector<std::int16_t>{11, -11},
            "Retail stereo ADPCM must decode interleaved channels with independent predictors");

    IlsfAdpcmDecoderState streaming_state = SoundSystem_InitializeIlsfAdpcmDecoder(1);
    std::array<std::int16_t, 1> first_mono_sample{};
    const std::array<std::uint8_t, 1> packed_nibbles = {0x7fU};
    const std::size_t first_bytes_consumed = SoundSystem_DecodeIlsfAdpcmFrames(
        streaming_state, packed_nibbles, first_mono_sample, 1);
    require(first_bytes_consumed == 1 && first_mono_sample[0] == 11 && streaming_state.has_pending_nibble,
            "A one-frame mono ADPCM chunk must retain its low nibble as streaming state");

    std::array<std::int16_t, 1> second_mono_sample{};
    const std::size_t second_bytes_consumed = SoundSystem_DecodeIlsfAdpcmFrames(
        streaming_state, std::span<const std::uint8_t>{}, second_mono_sample, 1);
    require(second_bytes_consumed == 0 && second_mono_sample[0] == -19 && !streaming_state.has_pending_nibble,
            "The next mono ADPCM chunk must consume the retained low nibble without rereading its byte");
    require(SoundSystem_DecodeIlsfAdpcmFrames(
                streaming_state, std::span<const std::uint8_t>{}, std::span<std::int16_t>{}, 0) == 0,
            "A zero-frame ADPCM read must preserve decoder state without consuming bytes");
}

class NonSeekableStringBuffer : public std::stringbuf
{
  public:
    explicit NonSeekableStringBuffer(std::string content)
        : std::stringbuf(std::move(content), std::ios::in)
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

void test_stream_reader_failures()
{
    const std::vector<std::uint8_t> stereo_bytes = make_ilsf(
        IlsfStorageFormat::streamed_pcm, 16, 2, 22050, 1, {0x01U, 0x00U, 0x02U, 0x00U});
    const std::string stereo_content(
        reinterpret_cast<const char*>(stereo_bytes.data()), stereo_bytes.size());
    std::istringstream stereo_stream(stereo_content, std::ios::binary);
    IlsfStreamReader stereo_reader = IlsfStreamReader::Open(stereo_stream);
    std::array<std::int16_t, 1> too_small_stereo_destination{};
    require_audio_exception(
        [&] { static_cast<void>(stereo_reader.ReadFrames(too_small_stereo_destination)); },
        "A stereo reader must reject destinations that cannot hold one complete frame");

    const std::vector<std::uint8_t> truncated_bytes = make_ilsf(
        IlsfStorageFormat::streamed_pcm, 16, 1, 22050, 2, {0x01U, 0x00U});
    const std::string truncated_content(
        reinterpret_cast<const char*>(truncated_bytes.data()), truncated_bytes.size());
    std::istringstream truncated_stream(truncated_content, std::ios::binary);
    IlsfStreamReader truncated_reader = IlsfStreamReader::Open(truncated_stream);
    std::array<std::int16_t, 2> truncated_output{};
    require_audio_exception(
        [&] { static_cast<void>(truncated_reader.ReadFrames(truncated_output)); },
        "A streamed sound ending before its declared frame count must fail clearly");

    NonSeekableStringBuffer non_seekable_buffer(stereo_content);
    std::istream non_seekable_stream(&non_seekable_buffer);
    IlsfStreamReader non_seekable_reader = IlsfStreamReader::Open(non_seekable_stream);
    require(!non_seekable_reader.CanRestart(), "Non-seekable sound streams must disable restart");
    require_audio_exception(
        [&] { non_seekable_reader.Restart(); },
        "Restarting a non-seekable sound stream must fail with AudioException");
}

void test_invalid_inputs()
{
    require(!SoundSystem_IsIlsfFile(std::array<std::uint8_t, 4>{'W', 'A', 'V', 'E'}),
            "RIFF/WAVE bytes must not be mistaken for a retail ILSF file");
    require_audio_exception(
        [] { static_cast<void>(SoundSystem_ParseIlsfFile(std::array<std::uint8_t, 3>{'I', 'L', 'S'})); },
        "A truncated ILSF header must fail with AudioException");

    std::vector<std::uint8_t> invalid_format = make_ilsf(
        IlsfStorageFormat::resident_pcm, 16, 1, 22050, 0, {});
    write_little_endian_u16(invalid_format, 4, 4);
    require_audio_exception(
        [&] { static_cast<void>(SoundSystem_ParseIlsfFile(invalid_format)); },
        "Unknown ILSF storage formats must fail with AudioException");

    const std::vector<std::uint8_t> invalid_channels = make_ilsf(
        IlsfStorageFormat::resident_pcm, 16, 3, 22050, 0, {});
    require_audio_exception(
        [&] { static_cast<void>(SoundSystem_ParseIlsfFile(invalid_channels)); },
        "ILSF channel counts above the retail two-channel bound must fail");

    const std::vector<std::uint8_t> invalid_adpcm_width = make_ilsf(
        IlsfStorageFormat::resident_adpcm, 8, 1, 22050, 0, {});
    require_audio_exception(
        [&] { static_cast<void>(SoundSystem_ParseIlsfFile(invalid_adpcm_width)); },
        "ILSF ADPCM must declare 16-bit decoded samples");

    const std::vector<std::uint8_t> truncated_pcm = make_ilsf(
        IlsfStorageFormat::resident_pcm, 16, 1, 22050, 2, {0x00U, 0x00U});
    require_audio_exception(
        [&] { static_cast<void>(SoundSystem_ParseIlsfFile(truncated_pcm)); },
        "ILSF payload shorter than its declared frames must fail");
}

void verify_retail_level_one_sounds(const std::filesystem::path& game_root)
{
    const std::filesystem::path sound_directory =
        game_root / "MISSIONS" / "location0" / "LEVEL1" / "SOUNDS";
    require(std::filesystem::is_directory(sound_directory),
            "Retail Level 1 sound directory is missing: " + sound_directory.string());

    std::size_t sound_file_count = 0;
    bool saw_streamed_music = false;
    bool saw_streamed_ambience = false;
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(sound_directory))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".wav")
        {
            continue;
        }

        const std::vector<std::uint8_t> bytes = read_file(entry.path());
        require(SoundSystem_IsIlsfFile(bytes),
                "Retail Level 1 .wav is not ILSF: " + entry.path().filename().string());
        const IlsfSoundData sound = SoundSystem_ParseIlsfFile(bytes);
        const std::vector<std::int16_t> decoded_samples = SoundSystem_DecodeIlsfFileToPcm16(sound);
        require(decoded_samples.size() == sound.DecodedSampleCount(),
                "Retail Level 1 sound decoded to an unexpected sample count: " +
                    entry.path().filename().string());

        const std::string stream_bytes(reinterpret_cast<const char*>(bytes.data()), bytes.size());
        std::istringstream source_stream(stream_bytes, std::ios::binary);
        IlsfStreamReader stream_reader = IlsfStreamReader::Open(source_stream);
        require(stream_reader.CanRestart(), "Retail sound test input must be seekable");
        std::vector<std::int16_t> streamed_samples;
        std::array<std::int16_t, 509> frame_buffer{};
        while (true)
        {
            const std::size_t frames_read = stream_reader.ReadFrames(frame_buffer);
            if (frames_read == 0)
            {
                break;
            }
            const std::size_t samples_read = frames_read * sound.channel_count;
            streamed_samples.insert(
                streamed_samples.end(), frame_buffer.begin(), frame_buffer.begin() +
                    static_cast<std::ptrdiff_t>(samples_read));
        }
        require(stream_reader.EndOfSound() && streamed_samples == decoded_samples,
                "Chunked Level 1 decoding differs from full-buffer decoding: " +
                    entry.path().filename().string());

        stream_reader.Restart();
        const std::size_t restarted_frame_count = stream_reader.ReadFrames(frame_buffer);
        const std::size_t restarted_sample_count = restarted_frame_count * sound.channel_count;
        require(stream_reader.FramesRead() == restarted_frame_count && restarted_frame_count > 0 &&
                    std::equal(frame_buffer.begin(),
                               frame_buffer.begin() + static_cast<std::ptrdiff_t>(restarted_sample_count),
                               decoded_samples.begin()),
                "Restarting a Level 1 sound did not reset its frame and ADPCM decoder state: " +
                    entry.path().filename().string());

        if (entry.path().filename() == "game_music.wav")
        {
            saw_streamed_music = sound.storage_format == IlsfStorageFormat::streamed_adpcm &&
                                 sound.channel_count == 2 && sound.sample_rate == 22050;
        }
        if (entry.path().filename() == "m1_ambience.wav")
        {
            saw_streamed_ambience = sound.storage_format == IlsfStorageFormat::streamed_pcm &&
                                    sound.channel_count == 2 && sound.sample_rate == 11025;
        }
        ++sound_file_count;
    }

    require(sound_file_count == 28, "Expected to verify all 28 retail Level 1 ILSF sound files");
    require(saw_streamed_music, "Retail Level 1 music ILSF metadata does not match its streamed stereo format");
    require(saw_streamed_ambience,
            "Retail Level 1 ambience ILSF metadata does not match its streamed stereo PCM format");
    std::cout << "Decoded and validated " << sound_file_count
              << " original Level 1 ILSF sound files\n";
}

} // namespace

int main(int argc, char** argv)
{
    try
    {
        test_all_storage_formats();
        test_pcm_decoding();
        test_adpcm_decoding();
        test_stream_reader_failures();
        test_invalid_inputs();
        if (argc > 1)
        {
            verify_retail_level_one_sounds(argv[1]);
        }

        std::cout << "Retail IGI.EXE ILSF parser and sample checks passed\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Retail ILSF parser and sample check failed: " << exception.what() << '\n';
        return 1;
    }
}
