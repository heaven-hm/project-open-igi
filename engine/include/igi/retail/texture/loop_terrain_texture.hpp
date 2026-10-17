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

    // Host adapter: parses a LOOP terrain texture container into a C++ LoopTerrainTextureFile struct.
    // The literal retail Terrain_TextureFileNotOfRequ (VA 0x005175A0) is in texture_loaders.hpp.
    // Tools: Ghidra/REA and Radare2; runtime parity is not verified.
    // Evidence: REA ev_949b22183952b274cb5402e31a7491f1ff8052c92b2457e7ec630aea93dc69ca;
    // RE-Agent catalog assets/RE_AGENT_VERIFIED_NATIVES.md (0x005175A0, CFG 1.000, 5/6).
    // CFG: radare2 13 blocks/20 edges; Binary Ninja 13 blocks; 92.6% structural agreement.
    // Confidence: partial host parser adapter; literal native dispatch is in texture_loaders.cpp.
    // Signature: std::span<const std::byte> -> LoopTerrainTextureFile differs from retail void*(int32_t, int32_t, const char*).
    // Usage/tests: Used by host LOOP texture inspection tests.
    [[nodiscard]] static LoopTerrainTextureFile
    Host_ParseTerrainTextureFile(std::span<const std::byte> file_bytes);
};

struct LoopModelTextureFile
{
    static constexpr std::uint32_t loop_signature = 0x504F4F4CU;

    std::uint32_t version = 0U;
    std::array<std::uint32_t, 6U> operation_arguments{};
    std::size_t pixel_data_offset = 0U;

    // Host adapter: parses a LOOP model texture container into a C++ LoopModelTextureFile struct.
    // The literal retail Sys_SIsNotAValidTextureF (VA 0x004B24C0) is in texture_loaders.hpp.
    // Tools: Ghidra/REA and Binary Ninja; runtime behavior is not verified.
    // Evidence: REA ev_133bd2573eab1beb3490cc8b39abb71ad6028c93b45eb5dc9b80cef6273f5214.
    // CFG: radare2 12 blocks/18 edges; Binary Ninja 12 blocks; 100% structural agreement.
    // Confidence: partial host parser adapter; literal native dispatch is in texture_loaders.cpp.
    // Signature: std::span<const std::byte> -> LoopModelTextureFile differs from retail int32_t(int32_t, int32_t, const char*).
    // Usage/tests: Used by host LOOP texture pixel decoder and inspection tests.
    [[nodiscard]] static LoopModelTextureFile Host_ParseModelTextureFile(std::span<const std::byte> file_bytes);
};

} // namespace igi::retail::texture
