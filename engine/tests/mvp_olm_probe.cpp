#include "olm_parser.h"
#include "res_parser.h"

#include <bit>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void write_unsigned_integer(std::vector<std::uint8_t>& bytes, std::size_t offset, std::uint32_t value)
{
    for (std::size_t byte_index = 0; byte_index < sizeof(value); ++byte_index)
    {
        bytes[offset + byte_index] = static_cast<std::uint8_t>(value >> (byte_index * 8));
    }
}

void write_unsigned_short(std::vector<std::uint8_t>& bytes, std::size_t offset, std::uint16_t value)
{
    bytes[offset] = static_cast<std::uint8_t>(value);
    bytes[offset + 1] = static_cast<std::uint8_t>(value >> 8);
}

std::vector<std::uint8_t> make_lightmap_fixture()
{
    constexpr std::size_t prefix_size = 60;
    constexpr std::size_t descriptor_size = 44;
    std::vector<std::uint8_t> bytes(prefix_size + descriptor_size * 2 + 12);
    write_unsigned_integer(bytes, 0, std::bit_cast<std::uint32_t>(0.12F));
    write_unsigned_integer(bytes, 44, 2);
    write_unsigned_short(bytes, prefix_size + 40, 2);
    write_unsigned_short(bytes, prefix_size + 42, 1);
    write_unsigned_short(bytes, prefix_size + descriptor_size + 40, 1);
    write_unsigned_short(bytes, prefix_size + descriptor_size + 42, 1);
    for (std::size_t byte_index = prefix_size + descriptor_size * 2; byte_index < bytes.size(); ++byte_index)
    {
        bytes[byte_index] = static_cast<std::uint8_t>(byte_index & 0xffU);
    }
    return bytes;
}

void verify_synthetic_lightmap()
{
    const auto fixture = make_lightmap_fixture();
    const auto lightmap = igi::assets::parse_object_lightmap(fixture);
    require(std::abs(lightmap.version - 0.12F) < 0.0001F, "OLM version must be preserved");
    require(lightmap.images.size() == 2, "OLM must parse every image descriptor");
    require(lightmap.images[0].width == 2 && lightmap.images[0].height == 1 &&
                lightmap.images[0].rgba_pixels.size() == 8,
            "First OLM image dimensions and RGBA bytes must match the fixture");
    require(lightmap.images[1].width == 1 && lightmap.images[1].height == 1 &&
                lightmap.images[1].rgba_pixels.size() == 4,
            "Second OLM image dimensions and RGBA bytes must match the fixture");
    require(lightmap.images[0].rgba_pixels.front() == fixture[148] &&
                lightmap.images[1].rgba_pixels.back() == fixture.back(),
            "OLM image payloads must remain in descriptor order");

    for (std::vector<std::uint8_t> malformed : {std::vector<std::uint8_t>(59),
                                                std::vector<std::uint8_t>(104),
                                                std::vector<std::uint8_t>(60 + 2 * 44 + 11)})
    {
        bool rejected = false;
        try
        {
            static_cast<void>(igi::assets::parse_object_lightmap(malformed));
        }
        catch (const std::exception&)
        {
            rejected = true;
        }
        require(rejected, "Truncated OLM input must be rejected");
    }

    auto trailing_bytes = fixture;
    trailing_bytes.push_back(0);
    bool rejected_trailing_data = false;
    try
    {
        static_cast<void>(igi::assets::parse_object_lightmap(trailing_bytes));
    }
    catch (const std::exception&)
    {
        rejected_trailing_data = true;
    }
    require(rejected_trailing_data, "Trailing OLM bytes must be rejected");

    std::string_view set_name;
    std::uint32_t image_index = 99;
    require(igi::assets::parse_object_lightmap_entry_name(
                "LOCAL:missions/location0/level1/lightmaps/obj00014_00000.olm", set_name, image_index),
            "RES OLM path must parse");
    require(set_name == "obj00014" && image_index == 0, "RES OLM set and image index must be preserved");
    require(!igi::assets::parse_object_lightmap_entry_name("obj00014_invalid.olm", set_name, image_index),
            "Malformed RES OLM name must be rejected");
}

void verify_level_one_lightmap(const std::filesystem::path& game_directory)
{
    const auto archive_path = game_directory / "missions/location0/level1/lightmaps/lightmaps.res";
    std::size_t parsed_lightmap_count = 0;
    std::string archive_error;
    const bool archive_valid = RES_ForEachEntry(
        archive_path.string(),
        [&](const std::string& entry_name, const std::uint8_t* entry_bytes, std::size_t entry_size)
        {
            std::string_view set_name;
            std::uint32_t image_index = 0;
            if (!igi::assets::parse_object_lightmap_entry_name(entry_name, set_name, image_index))
            {
                return;
            }
            const auto entry_span = std::span<const std::uint8_t>(entry_bytes, entry_size);
            const auto lightmap = igi::assets::parse_object_lightmap(entry_span);
            require(!set_name.empty(), "Retail OLM resource set name must not be empty");
            require(std::abs(lightmap.version - 0.12F) < 0.0001F,
                    "Retail Level 1 OLM versions must match converter inspection");
            require(!lightmap.images.empty(), "Retail Level 1 OLM resource must contain image data");
            ++parsed_lightmap_count;
        },
        archive_error);
    require(archive_valid,
            archive_error.empty() ? "Level 1 lightmaps.res must be readable" : archive_error.c_str());
    require(parsed_lightmap_count > 0, "Level 1 lightmaps.res must contain OLM resources");

    const auto sample_bytes =
        RES_Extract(archive_path.string(), "LOCAL:missions/location0/level1/lightmaps/obj00014_00000.olm");
    require(!sample_bytes.empty(), "Level 1 lightmaps.res must contain obj00014_00000.olm");
    const auto sample_lightmap = igi::assets::parse_object_lightmap(sample_bytes);
    require(sample_lightmap.images.size() == 1, "Retail Level 1 sample OLM must contain one image");
    require(sample_lightmap.images.front().width == 256 && sample_lightmap.images.front().height == 64,
            "Retail Level 1 sample OLM dimensions must match converter inspection");
    require(sample_lightmap.images.front().rgba_pixels.size() == 256U * 64U * 4U,
            "Retail Level 1 sample OLM payload size must match its RGBA dimensions");
    std::cout << "[PASS] Retail Level 1 OLM: " << archive_path << ", parsed " << parsed_lightmap_count
              << " resources; sample image " << sample_lightmap.images.front().width << 'x'
              << sample_lightmap.images.front().height << " RGBA\n";
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        verify_synthetic_lightmap();
        std::cout << "[PASS] Synthetic OLM parsing, malformed input rejection, and RES entry naming\n";
        if (argument_count == 2)
        {
            verify_level_one_lightmap(arguments[1]);
        }
        else if (argument_count != 1)
        {
            throw std::runtime_error("Usage: mvp_olm_probe [IGI game directory]");
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[FAIL] OLM probe: " << exception.what() << '\n';
        return 1;
    }
    return 0;
}
