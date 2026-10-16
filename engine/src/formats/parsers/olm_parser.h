#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace igi::assets
{

struct ObjectLightmapImage final
{
    std::uint16_t width{};
    std::uint16_t height{};
    std::vector<std::uint8_t> rgba_pixels;
};

struct ObjectLightmap final
{
    float version{};
    std::vector<ObjectLightmapImage> images;
};

[[nodiscard]] ObjectLightmap parse_object_lightmap(std::span<const std::uint8_t> bytes);

[[nodiscard]] bool parse_object_lightmap_entry_name(std::string_view entry_name,
                                                    std::string_view& set_name,
                                                    std::uint32_t& image_index) noexcept;

} // namespace igi::assets
