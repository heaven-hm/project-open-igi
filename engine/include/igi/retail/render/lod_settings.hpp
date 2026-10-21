#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace igi::retail::render {

// Retail dyn-object record written by LODSettings_LoadParameters (0x004EDB60):
//   +0x24  int32 flag set to 1 when the LOD settings were applied
//   +0x28  five float32 LOD distances in retail units (the context distances * 4096)
// The port models the record as the retail byte range with named accessors.
struct LODSettingsRecord
{
    static constexpr std::size_t byte_size = 0x40;
    static constexpr std::size_t flag_offset = 0x24;
    static constexpr std::size_t distances_offset = 0x28;
    static constexpr std::size_t distance_count = 5;

    alignas(8) std::array<std::uint8_t, byte_size> bytes{};

    [[nodiscard]] std::int32_t applied_flag() const noexcept;
    [[nodiscard]] float* distances() noexcept;
    [[nodiscard]] const float* distances() const noexcept;
};

static_assert(sizeof(LODSettingsRecord) == LODSettingsRecord::byte_size,
              "LODSettingsRecord must keep the retail backend layout");

// Retail resolves the task's dyn object through DynObj_Missing (0x004CEC10), which looks the
// name stored at context+0x20 up in the process dyn-object registry. The port passes that
// registry lookup in as a host boundary: the lookup returns the record for a name, or nullptr
// when no such object exists (the case the retail fixture exercises with an empty registry).
using LODSettingsLookup = LODSettingsRecord* (*)(const char* name) noexcept;

// 0x004EDB60 LODSettings_LoadParameters: clamps the five float32 distance slots at
// context+0x30..0x43 so they never decrease, then resolves the dyn object named at
// context+0x20 and, when it exists, marks it applied and stores each distance * 4096.
// Returns the resolved record (null when the name resolves to nothing).
[[nodiscard]] LODSettingsRecord* LODSettings_LoadParameters(void* context,
                                                            LODSettingsLookup lookup) noexcept;

} // namespace igi::retail::render
