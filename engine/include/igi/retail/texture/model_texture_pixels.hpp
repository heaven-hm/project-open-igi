#pragma once
#include "igi/retail/texture/loop_terrain_texture.hpp"

namespace igi::retail::texture
{
struct ModelTexturePixels
{
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::vector<std::uint32_t> argb_pixels;
};
// Texture upload registered at runtime: DownloadTexture -> 0x0050AD90.
// Static pixel converters: 0x00517C90 (ARGB1555), 0x00517DD0 (RGB565), 0x00517F00 (32-bit copy).
// Original catalogue labels describe unrelated terrain/physics operations and are provisional.
// Scope: base-level pixel conversion only. Palette and mip-chain upload remain separate.
// Verified by retail callback-address read and radare2/CLEAN static bodies; visual parity pending.
[[nodiscard]] ModelTexturePixels Texture_DecodeModelPixels(std::span<const std::byte> file_bytes);
} // namespace igi::retail::texture
