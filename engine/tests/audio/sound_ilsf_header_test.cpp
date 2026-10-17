#include "igi/retail/audio/sound_ilsf_header.hpp"

#include <array>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string_view>

namespace {

void require(bool condition, std::string_view description)
{
    if (!condition) {
        std::cerr << "FAIL: " << description << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void test_level_one_ambience_header_fields()
{
    constexpr std::array<std::byte, 20U> header_bytes = {
        std::byte{ 'I' }, std::byte{ 'L' }, std::byte{ 'S' }, std::byte{ 'F' },
        std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x10 }, std::byte{ 0x00 },
        std::byte{ 0x02 }, std::byte{ 0x00 }, std::byte{ 0x00 }, std::byte{ 0x00 },
        std::byte{ 0x11 }, std::byte{ 0x2B }, std::byte{ 0x00 }, std::byte{ 0x00 },
        std::byte{ 0x1F }, std::byte{ 0x8C }, std::byte{ 0x05 }, std::byte{ 0x00 },
    };

    const igi::retail::audio::SoundIlsfHeaderReadResult result =
        igi::retail::audio::Sound_ReadIlsfHeader(header_bytes);
    require(result.succeeded(), "Level 1 ambience ILSF header is accepted");
    require(result.header.storage_format == 0U, "Level 1 ambience storage code is retained");
    require(result.header.decoded_bits_per_sample == 16U, "Level 1 ambience sample width is decoded");
    require(result.header.channel_count == 2U, "Level 1 ambience channel count is decoded");
    require(result.header.reserved_value == 0U, "Level 1 ambience reserved field is retained");
    require(result.header.sample_rate_hertz == 11025U, "Level 1 ambience sample rate is decoded");
    require(result.header.frame_count == 363551U, "Level 1 ambience frame count is decoded");
}

void test_header_reader_rejects_short_input_before_reading()
{
    constexpr std::array<std::byte, 19U> short_input{};
    const igi::retail::audio::SoundIlsfHeaderReadResult result =
        igi::retail::audio::Sound_ReadIlsfHeader(short_input);
    require(result.error == igi::retail::audio::SoundIlsfHeaderError::header_too_short,
        "short header is rejected before field access");
}

void test_header_reader_checks_only_the_observed_signature()
{
    std::array<std::byte, 20U> header_bytes{};
    header_bytes[0] = std::byte{ 'N' };
    const igi::retail::audio::SoundIlsfHeaderReadResult invalid_signature =
        igi::retail::audio::Sound_ReadIlsfHeader(header_bytes);
    require(invalid_signature.error == igi::retail::audio::SoundIlsfHeaderError::invalid_signature,
        "non-ILSF signature is rejected");

    header_bytes[0] = std::byte{ 'I' };
    header_bytes[1] = std::byte{ 'L' };
    header_bytes[2] = std::byte{ 'S' };
    header_bytes[3] = std::byte{ 'F' };
    header_bytes[4] = std::byte{ 0xFF };
    header_bytes[5] = std::byte{ 0xFF };
    const igi::retail::audio::SoundIlsfHeaderReadResult unclassified_format =
        igi::retail::audio::Sound_ReadIlsfHeader(header_bytes);
    require(unclassified_format.succeeded(), "header reader preserves unknown format codes without invented filtering");
    require(unclassified_format.header.storage_format == 65535U, "unknown format code is preserved verbatim");
}

} // namespace

int main()
{
    test_level_one_ambience_header_fields();
    test_header_reader_rejects_short_input_before_reading();
    test_header_reader_checks_only_the_observed_signature();
    std::cout << "Retail ILSF header tests passed.\n";
    return EXIT_SUCCESS;
}
