#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace igi::retail::config {

// The seven performance levels the retail config keeps as flag bitmasks, read from the dword
// table at 0x00537B74 (the walk in 0x00405530/0x004055C0 ends at 0x00537B90).
inline constexpr std::array<std::uint32_t, 7> performance_flag_table{
    0x19EU, 0x1AEU, 0x0DU, 0x60U, 0x4DU, 0x41U, 0x00U};
inline constexpr std::size_t performance_level_count = performance_flag_table.size();

// Script type descriptor the config natives stamp into every value block they return
// (the dword at 0x00567C74, whose content is zero: it is an address used as an opaque type
// handle by the script VM).
inline constexpr std::uint32_t script_real_type_descriptor = 0x00567C74U;

// The 24-byte script value block the natives copy to their context argument with rep movsd x6:
//   +0x00 int32 value
//   +0x04 undefined dword (retail never writes it for the flag reads)
//   +0x08 float64 value
//   +0x10 type descriptor dword
struct ScriptValueBlock
{
    std::int32_t integer = 0;
    std::uint32_t undefined_slot = 0;
    double real = 0.0;
    std::uint32_t type_descriptor = 0;
};

static_assert(sizeof(ScriptValueBlock) == 0x18, "script value block must be the retail size");
static_assert(offsetof(ScriptValueBlock, real) == 0x08, "retail double slot is +0x08");
static_assert(offsetof(ScriptValueBlock, type_descriptor) == 0x10, "retail type slot is +0x10");

// Active profile graphic options record. Retail keeps the profile records at 0x00BC2394 with
// the active index at 0x00BC2384 and a stride of 0x564 bytes; the performance flags sit at
// +0x0C. Only that dword is read or written by the natives in this file.
struct GraphicOptionsRecord
{
    static constexpr std::size_t byte_size = 0x564;
    static constexpr std::size_t performance_flags_offset = 0x0C;

    std::array<std::uint8_t, byte_size> bytes{};

    [[nodiscard]] std::int32_t performance_flags() const noexcept;
    void set_performance_flags(std::int32_t flags) noexcept;
};

static_assert(sizeof(GraphicOptionsRecord) == GraphicOptionsRecord::byte_size,
              "graphic options record must keep the retail stride");

// Host mirror of the retail config globals (the active index plus the record array). The port
// keeps a bounded array; the accessor clamps the index, which differs from the unchecked
// retail multiply and is recorded as a limitation.
class GraphicOptionsStore
{
  public:
    static constexpr std::size_t profile_capacity = 8;

    [[nodiscard]] static GraphicOptionsStore& active() noexcept;

    [[nodiscard]] std::uint32_t active_index() const noexcept { return active_index_; }
    void set_active_index(std::uint32_t index) noexcept;
    [[nodiscard]] GraphicOptionsRecord& record(std::uint32_t index) noexcept;
    [[nodiscard]] const GraphicOptionsRecord& record(std::uint32_t index) const noexcept;

  private:
    std::array<GraphicOptionsRecord, profile_capacity> records_{};
    std::uint32_t active_index_ = 0;
};

// 0x00404590 Config_GetActiveGraphicOptions: returns the record of the active profile
// (retail computes the base as index * 0x564 + 0x00BC2394).
[[nodiscard]] GraphicOptionsRecord* Config_GetActiveGraphicOptions() noexcept;

// 0x00404F00 Config_GraphicOptionsSetPerformanceFlags: stores the resolved integer on the
// active record (+0x0C) and writes the value block {1, undefined, 1.0, type} to the context.
// Retail reads the integer through Script_GetArg; the host interface takes it resolved.
void Config_GraphicOptionsSetPerformanceFlags(void* context, std::int32_t flags) noexcept;

// 0x00404F60 Config_GraphicOptionsGetPerformanceFlags: writes the active flags as both the
// integer and the float64 of a value block {flags, undefined, double(flags), type}.
void Config_GraphicOptionsGetPerformanceFlags(void* context) noexcept;

// 0x00405620 Config_GraphicOptionsNumPerfLevels: writes the constant value block
// {7, undefined, 7.0, type} to the context.
void Config_GraphicOptionsNumPerfLevels(void* context) noexcept;

// 0x004055C0 Config_GraphicOptionsGetPerfFlagsFromLevel: writes the value block for the flag
// bitmask of the requested performance level ({table[level], undefined, double(table[level]),
// type}).
void Config_GraphicOptionsGetPerfFlagsFromLevel(void* context, std::int32_t level) noexcept;

// 0x00405530 Config_GraphicOptionsGetPerfLevelFromFlags: scans the flag table and writes the
// value block for the level whose bitmask is closest by popcount, lowest level winning ties.
void Config_GraphicOptionsGetPerfLevelFromFlags(void* context, std::int32_t flags) noexcept;

} // namespace igi::retail::config
