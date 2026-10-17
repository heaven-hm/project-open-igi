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

// Is_ResourceLoaded - original IGI.EXE native at VA 0x004B5B90.
// Static behavior: resolves aliases/separators and checks the loaded-resource registry.
// REA evidence: ev_d3b8fb4bf158da1ff83b3a8ed53d147cb06f2afc585d7dd6d92a36c74d899e2f.
[[nodiscard]] ResourceHandle Is_ResourceLoaded(
    const ResourceCache& resource_cache,
    const std::filesystem::path& resource_path);

// Resource_Load - original IGI.EXE native at VA 0x004B5F00.
// Verification: retail runtime + REA/Ghidra confirm the optional output is a 32-bit byte count.
// Retail returns the byte pointer; this host adapter retains ownership in ResourceHandle::data().
// Archive unpacking, aliases, and allocator globals remain unported; full parity is unverified.
// REA evidence: ev_6f2afb3388ef8a6717637dc103d353963648cc8aedba9d6a88ab27b65fd34c1e.
[[nodiscard]] ResourceHandle Resource_Load(
    ResourceCache& resource_cache,
    const std::filesystem::path& resource_path,
    std::uint32_t* out_byte_count = nullptr);

// Resource_Unload - original IGI.EXE native at VA 0x004B6380.
// Removes the registry's reference; active handles keep their data address valid.
[[nodiscard]] bool Resource_Unload(
    ResourceCache& resource_cache,
    const std::filesystem::path& resource_path);

} // namespace igi::retail::resource
