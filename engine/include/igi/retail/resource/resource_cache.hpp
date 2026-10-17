#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace igi::retail::resource {

class ResourceLoadError final : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct LoadedResource {
    std::string resource_path;
    std::vector<std::byte> bytes;

    [[nodiscard]] const std::byte* data() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;
};

using ResourceHandle = std::shared_ptr<const LoadedResource>;

class ResourceCache {
public:
    [[nodiscard]] std::size_t size() const noexcept;

private:
    friend ResourceHandle Resource_Load(
        ResourceCache& resource_cache,
        const std::filesystem::path& resource_path,
        std::uint32_t* out_byte_count);
    friend ResourceHandle Is_ResourceLoaded(
        const ResourceCache& resource_cache,
        const std::filesystem::path& resource_path);
    friend bool Resource_Unload(
        ResourceCache& resource_cache,
        const std::filesystem::path& resource_path);

    std::unordered_map<std::string, ResourceHandle> loaded_resources_;
};

// Native: Is_ResourceLoaded() - VA 0x004B5B90 - partial adapter, not a complete literal port.
// Behavior: Resolves aliases/separators, searches the native registry and optionally outputs its entry.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_d58a0427b58e2d3d638c5f4bd2c1332506b572ba79e84fd3fa46f4f3b1d5118f.
// CFG: r2 blocks=24, edges=35; BN blocks=24; source agreement=100.0%.
// Confidence: partial cache adapter; native alias search/hash/list semantics are not fully transcribed.
// Signature: Retail flag/out-entry interface is adapted to a C++ ResourceHandle lookup.
// Usage/tests: Used by resource cache tests/API; cache fixtures passed; native call-by-call replay pending.
[[nodiscard]] ResourceHandle Is_ResourceLoaded(
    const ResourceCache& resource_cache,
    const std::filesystem::path& resource_path);

// Native: Resource_Load() - VA 0x004B5F00 - partial adapter, not a complete literal port.
// Behavior: Loads cached/unpacked bytes and optionally writes their uint32 byte count.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_87092f5147addf2b19ea080dbc70a44d1218ec706e022f019e4ba19bad366057.
// CFG: r2 blocks=10, edges=12; BN blocks=10; source agreement=100.0%.
// Confidence: partial host adapter; retail allocator/VFS/registry behavior is incomplete.
// Signature: C++ ResourceHandle/ResourceArchive signatures adapt the retail raw-pointer API.
// Usage/tests: Used by model/QVM loading; resource fixtures and model-scene runtime passed.
[[nodiscard]] ResourceHandle Resource_Load(
    ResourceCache& resource_cache,
    const std::filesystem::path& resource_path,
    std::uint32_t* out_byte_count = nullptr);

// Native: Resource_Unload() - VA 0x004B6380 - partial adapter, not a complete literal port.
// Behavior: Finds a resource, conditionally releases supported entry types, and errors if absent.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_61eb49f6f63a80b807875b80fe9cebc7ec3cf1637b5215987b4858715812e9bd.
// CFG: r2 blocks=7, edges=9; BN blocks=7; source agreement=100.0%.
// Confidence: partial adapter; host false-on-missing differs from the native fatal-error path.
// Signature: Retail void/path API is adapted to bool/cache/path; native ABI is not preserved yet.
// Usage/tests: Used by resource lifecycle fixtures; handle lifetime passed; retail unload replay pending.
[[nodiscard]] bool Resource_Unload(
    ResourceCache& resource_cache,
    const std::filesystem::path& resource_path);

} // namespace igi::retail::resource
