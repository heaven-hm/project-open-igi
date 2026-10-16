#include "olm_parser.h"

#include <algorithm>
#include <bit>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace igi::assets
{
namespace
{

constexpr std::size_t prefix_size = 60;
constexpr std::size_t image_descriptor_size = 44;
constexpr std::size_t image_dimensions_offset = 40;
constexpr std::uint32_t maximum_image_count = 64;

std::uint16_t read_unsigned_short(std::span<const std::uint8_t> bytes, std::size_t offset)
{
    if (offset > bytes.size() || bytes.size() - offset < sizeof(std::uint16_t))
    {
        throw std::runtime_error("Object lightmap is truncated while reading an image dimension");
    }
    return static_cast<std::uint16_t>(bytes[offset]) |
           static_cast<std::uint16_t>(static_cast<std::uint16_t>(bytes[offset + 1]) << 8);
}

std::uint32_t read_unsigned_integer(std::span<const std::uint8_t> bytes, std::size_t offset)
{
    if (offset > bytes.size() || bytes.size() - offset < sizeof(std::uint32_t))
    {
        throw std::runtime_error("Object lightmap is truncated while reading its image count");
    }
    return static_cast<std::uint32_t>(bytes[offset]) | (static_cast<std::uint32_t>(bytes[offset + 1]) << 8) |
           (static_cast<std::uint32_t>(bytes[offset + 2]) << 16) |
           (static_cast<std::uint32_t>(bytes[offset + 3]) << 24);
}

bool ends_with_case_insensitive(std::string_view value, std::string_view suffix) noexcept
{
    if (value.size() < suffix.size())
    {
        return false;
    }
    const auto value_suffix = value.substr(value.size() - suffix.size());
    return std::equal(value_suffix.begin(),
                      value_suffix.end(),
                      suffix.begin(),
                      suffix.end(),
                      [](char left, char right)
                      {
                          return std::tolower(static_cast<unsigned char>(left)) ==
                                 std::tolower(static_cast<unsigned char>(right));
                      });
}

} // namespace

ObjectLightmap parse_object_lightmap(std::span<const std::uint8_t> bytes)
{
    constexpr std::size_t minimum_file_size = prefix_size + image_descriptor_size;
    if (bytes.size() < minimum_file_size)
    {
        throw std::runtime_error("Object lightmap is shorter than its prefix and first image descriptor");
    }

    ObjectLightmap lightmap;
    lightmap.version = std::bit_cast<float>(read_unsigned_integer(bytes, 0));
    const std::uint32_t image_count = read_unsigned_integer(bytes, 44);
    if (image_count == 0 || image_count > maximum_image_count)
    {
        throw std::runtime_error("Object lightmap declares an unsupported image count");
    }

    const std::size_t descriptor_bytes = static_cast<std::size_t>(image_count) * image_descriptor_size;
    const std::size_t pixel_data_offset = prefix_size + descriptor_bytes;
    if (pixel_data_offset > bytes.size())
    {
        throw std::runtime_error("Object lightmap image descriptors exceed the file size");
    }

    lightmap.images.reserve(image_count);
    std::size_t pixel_offset = pixel_data_offset;
    for (std::size_t image_index = 0; image_index < image_count; ++image_index)
    {
        const std::size_t descriptor_offset = prefix_size + image_index * image_descriptor_size;
        const std::size_t dimensions_offset = descriptor_offset + image_dimensions_offset;
        const std::uint16_t width = read_unsigned_short(bytes, dimensions_offset);
        const std::uint16_t height = read_unsigned_short(bytes, dimensions_offset + 2);
        const std::uint64_t pixel_count = static_cast<std::uint64_t>(width) * height;
        const std::size_t remaining_pixel_capacity = (bytes.size() - pixel_offset) / 4;
        if (width == 0 || height == 0 || pixel_count > remaining_pixel_capacity)
        {
            throw std::runtime_error("Object lightmap pixel data exceeds the remaining file size");
        }
        const std::size_t pixel_bytes = static_cast<std::size_t>(pixel_count) * 4;
        ObjectLightmapImage image;
        image.width = width;
        image.height = height;
        image.rgba_pixels.assign(bytes.begin() + static_cast<std::ptrdiff_t>(pixel_offset),
                                 bytes.begin() + static_cast<std::ptrdiff_t>(pixel_offset + pixel_bytes));
        lightmap.images.push_back(std::move(image));
        pixel_offset += pixel_bytes;
    }
    if (pixel_offset != bytes.size())
    {
        throw std::runtime_error("Object lightmap contains trailing bytes after its image data");
    }
    return lightmap;
}

bool parse_object_lightmap_entry_name(std::string_view entry_name,
                                      std::string_view& set_name,
                                      std::uint32_t& image_index) noexcept
{
    const std::size_t separator = entry_name.find_last_of("/\\");
    if (separator != std::string_view::npos)
    {
        entry_name.remove_prefix(separator + 1);
    }
    if (!ends_with_case_insensitive(entry_name, ".olm"))
    {
        return false;
    }
    entry_name.remove_suffix(4);

    const std::size_t index_separator = entry_name.find_last_of('_');
    if (index_separator == std::string_view::npos || index_separator == 0 ||
        index_separator + 1 == entry_name.size())
    {
        return false;
    }

    std::uint32_t parsed_index = 0;
    const auto parsed = std::from_chars(
        entry_name.data() + index_separator + 1, entry_name.data() + entry_name.size(), parsed_index);
    if (parsed.ec != std::errc{} || parsed.ptr != entry_name.data() + entry_name.size())
    {
        return false;
    }
    set_name = entry_name.substr(0, index_separator);
    image_index = parsed_index;
    return true;
}

} // namespace igi::assets
