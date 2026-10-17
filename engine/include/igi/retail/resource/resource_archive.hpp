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

// Native: Sys_Resource() - VA 0x004B5F80 - partial adapter, not a complete literal port.
// Behavior: Recognizes ILFF/IRES, legacy RESOURCE entries, and the native raw-resource fallback.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_756c70344e9eb0cff58f5a134a0308a81a96d2483abaf4a70ae305f706b05829.
// CFG: r2 blocks=11, edges=14; BN blocks=11; source agreement=100.0%.
// Confidence: partial archive adapter; raw fallback and intrusive registry insertion remain unported.
// Signature: C++ span/name -> ResourceArchive differs from the retail registry/buffer interface.
// Usage/tests: Used to mount model/texture RES archives; archive fixtures passed; full parity pending.
[[nodiscard]] ResourceArchive Sys_Resource(
    std::span<const std::byte> resource_bytes,
    std::string_view source_name);

// Native: Resource_Load() - VA 0x004B5F00 - partial adapter, not a complete literal port.
// Behavior: Loads cached/unpacked bytes and optionally writes their uint32 byte count.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_87092f5147addf2b19ea080dbc70a44d1218ec706e022f019e4ba19bad366057.
// CFG: r2 blocks=10, edges=12; BN blocks=10; source agreement=100.0%.
// Confidence: partial host adapter; retail allocator/VFS/registry behavior is incomplete.
// Signature: C++ ResourceHandle/ResourceArchive signatures adapt the retail raw-pointer API.
// Usage/tests: Used by model/QVM loading; resource fixtures and model-scene runtime passed.
[[nodiscard]] ResourceArchive Resource_Load(const std::filesystem::path& resource_path);

} // namespace igi::retail::resource
