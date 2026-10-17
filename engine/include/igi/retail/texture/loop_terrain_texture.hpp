#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace igi::retail::texture {

class LoopTextureFormatError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct LoopTerrainTextureRecord {
    std::uint32_t data_offset = 0U;
    std::uint32_t shared_parameter = 0U;
    std::array<std::uint32_t, 3U> version_seven_parameters{};
    std::array<std::uint16_t, 3U> version_nine_parameters{};
};

struct LoopTerrainTextureFile {
    static constexpr std::uint32_t loop_signature = 0x504F4F4CU;

    std::uint32_t version = 0U;
    std::uint32_t palette_offset = 0U;
    std::uint32_t palette_size = 0U;
    std::uint32_t texture_count = 0U;
    std::uint32_t palette_operation_value = 2U;
    std::vector<std::byte> palette_bytes;
    std::vector<LoopTerrainTextureRecord> records;

    // Original IGI.EXE native: Terrain_TextureFileNotOfRequ at VA 0x005175A0.
    // The native builds dispatch records for 0x004B23B0; pixel upload is performed
    // by registered renderer operations and is not part of this parser result.
    [[nodiscard]] static LoopTerrainTextureFile Terrain_TextureFileNotOfRequ(
        std::span<const std::byte> file_bytes);
};

} // namespace igi::retail::texture
