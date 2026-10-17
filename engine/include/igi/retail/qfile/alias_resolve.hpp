#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::qfile {

// Device/alias table entry, laid out exactly as the retail walk expects:
//   +0x00  device name (NUL-terminated, compared with strncmp)
//   +0x10  alias string (semicolon separated)
//   +0x90  active flag (entry is skipped when zero)
// Entries are 0x94 bytes apart; the retail table spans 0x00942330..0x009435B0.
struct DeviceEntry {
    char name[0x10];
    char alias[0x80];
    std::int32_t active;
};

static_assert(sizeof(DeviceEntry) == 0x94U, "device entry stride must match the retail table");

inline constexpr std::size_t device_entry_count = (0x009435B0U - 0x00942330U) / 0x94U;

// 0x004B1020 - QFile_AliasResolve.
// Replaces the leading device name of `alias` with the device's alias path, appending the rest
// of `alias`. `depth` selects which semicolon-separated alias segment to use. Returns depth + 1
// when the chosen segment has a remainder, otherwise 0. When no device matches, `alias` is
// copied unchanged and 0 is returned. The resolved text is also stored in the global alias
// buffer at 0x00943A18.
std::int32_t QFile_AliasResolve(char* destination, const char* alias, std::int32_t depth);

// Port-owned storage for the retail device table (0x00942330) and alias buffer (0x00943A18).
[[nodiscard]] DeviceEntry* QFile_GetDeviceTable();
[[nodiscard]] const char* QFile_GetResolvedAlias();

} // namespace igi::retail::qfile
