#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace igi::retail::texture
{

class LoopTextureFormatError final : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

struct LoopTerrainTextureRecord
{
    std::uint32_t data_offset = 0U;
    std::uint32_t shared_parameter = 0U;
    std::array<std::uint32_t, 3U> version_seven_parameters{};
    std::array<std::uint16_t, 3U> version_nine_parameters{};
};

struct LoopTerrainTextureFile
{
    static constexpr std::uint32_t loop_signature = 0x504F4F4CU;

    std::uint32_t version = 0U;
    std::uint32_t palette_offset = 0U;
    std::uint32_t palette_size = 0U;
    std::uint32_t texture_count = 0U;
    std::uint32_t palette_operation_value = 2U;
    std::vector<std::byte> palette_bytes;
    std::vector<LoopTerrainTextureRecord> records;

    // Terrain_TextureFileNotOfRequ - 0x005175A0 - Native static evidence: CFG 1.000 (PASS), RE-Agent 5/6; port runtime parity unverified.
    // Tools: Ghidra/REA and Radare2; runtime parity is not verified.
    // Evidence: REA ev_949b22183952b274cb5402e31a7491f1ff8052c92b2457e7ec630aea93dc69ca;
    // RE-Agent catalog assets/RE_AGENT_VERIFIED_NATIVES.md (0x005175A0, CFG 1.000, 5/6).
    // The catalog name is provisional: REA labels this procedure retail routine 0x005175A0.
    // The native builds dispatch records for 0x004B23B0; pixel upload is performed
    // by registered renderer operations and is not part of this parser result.
    [[nodiscard]] static LoopTerrainTextureFile
    Terrain_TextureFileNotOfRequ(std::span<const std::byte> file_bytes);
};

struct LoopModelTextureFile
{
    static constexpr std::uint32_t loop_signature = 0x504F4F4CU;

    std::uint32_t version = 0U;
    std::array<std::uint32_t, 6U> operation_arguments{};
    std::size_t pixel_data_offset = 0U;

    // Texture_LoadModelFile - 0x004B24C0 - Static analysis only; symbol and CFG parity are not verified.
    // Tools: Ghidra/REA and Binary Ninja; runtime behavior is not verified.
    // Evidence: REA ev_133bd2573eab1beb3490cc8b39abb71ad6028c93b45eb5dc9b80cef6273f5214.
    // This semantic name is descriptive, not a confirmed retail symbol.
    [[nodiscard]] static LoopModelTextureFile Texture_LoadModelFile(std::span<const std::byte> file_bytes);
};

} // namespace igi::retail::texture
