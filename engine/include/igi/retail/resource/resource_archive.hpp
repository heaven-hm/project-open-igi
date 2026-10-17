#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace igi::retail::resource {

class ResourceFormatError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct ResourceEntry {
    std::string name;
    std::vector<std::byte> body;
};

class ResourceArchive {
public:
    static constexpr std::uint32_t ilff_signature = 0x46464C49U;
    static constexpr std::uint32_t ires_form = 0x53455249U;
    static constexpr std::uint32_t name_chunk = 0x454D414EU;
    static constexpr std::uint32_t body_chunk = 0x59444F42U;

    std::uint32_t form_type = 0;
    std::vector<ResourceEntry> entries;

    [[nodiscard]] const ResourceEntry* find(std::string_view resource_name) const noexcept;
};

// Original IGI.EXE Sys_Resource at VA 0x004B5F80.
// REA/Ghidra evidence ev_6f2afb3388ef8a6717637dc103d353963648cc8aedba9d6a88ab27b65fd34c1e:
// validates ILFF/IRES, then walks NAME/BODY resources in sibling order.
// This host-side adapter retains the archive entries; it does not reproduce the retail cache globals.
[[nodiscard]] ResourceArchive Sys_Resource(
    std::span<const std::byte> resource_bytes,
    std::string_view source_name);

// Original IGI.EXE Resource_Load at VA 0x004B5F00.
// This path adapter reads a host file and feeds its bytes through Sys_Resource. Retail cache,
// compression and allocation behavior remain separate engine work.
[[nodiscard]] ResourceArchive Resource_Load(const std::filesystem::path& resource_path);

} // namespace igi::retail::resource
