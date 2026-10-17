#include "igi/retail/resource/resource_cache.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <string_view>

namespace igi::retail::resource {
namespace {

[[nodiscard]] std::string normalize_resource_path(const std::filesystem::path& resource_path)
{
    std::string normalized_path = resource_path.lexically_normal().generic_string();
    std::replace(normalized_path.begin(), normalized_path.end(), '\\', '/');
    return normalized_path;
}

[[nodiscard]] std::vector<std::byte> read_resource_file(
    const std::filesystem::path& resource_path)
{
    std::ifstream resource_stream(resource_path, std::ios::binary | std::ios::ate);
    if (!resource_stream) {
        throw ResourceLoadError("Failed to load resource: '" + resource_path.string() + "'");
    }

    const std::streamoff resource_file_size = resource_stream.tellg();
    if (resource_file_size <= 0
        || static_cast<std::uintmax_t>(resource_file_size) > std::numeric_limits<std::uint32_t>::max()
        || static_cast<std::uintmax_t>(resource_file_size)
            > static_cast<std::uintmax_t>(std::numeric_limits<std::streamsize>::max())) {
        throw ResourceLoadError("Resource has an invalid or unsupported size: '"
            + resource_path.string() + "'");
    }

    std::vector<std::byte> resource_bytes(static_cast<std::size_t>(resource_file_size));
    resource_stream.seekg(0, std::ios::beg);
    resource_stream.read(
        reinterpret_cast<char*>(resource_bytes.data()),
        static_cast<std::streamsize>(resource_bytes.size()));
    if (!resource_stream) {
        throw ResourceLoadError("Failed while reading resource: '" + resource_path.string() + "'");
    }
    return resource_bytes;
}

} // namespace

const std::byte* LoadedResource::data() const noexcept
{
    return bytes.data();
}

std::size_t LoadedResource::size() const noexcept
{
    return bytes.size();
}

std::size_t ResourceCache::size() const noexcept
{
    return loaded_resources_.size();
}

// Host adapter: probes the host ResourceCache map by normalized path.
// The literal retail Is_ResourceLoaded (VA 0x004B5B90) is in is_resource_loaded.cpp.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_d58a0427b58e2d3d638c5f4bd2c1332506b572ba79e84fd3fa46f4f3b1d5118f.
// CFG: r2 blocks=24, edges=35; BN blocks=24; source agreement=100.0%.
// Confidence: partial cache adapter for host diagnostic harness.
// Signature: Retail flag/out-entry interface is adapted to a C++ ResourceHandle lookup.
// Usage/tests: Used by resource cache tests/API; cache fixtures passed.
ResourceHandle Host_FindCachedResource(
    const ResourceCache& resource_cache,
    const std::filesystem::path& resource_path)
{
    const auto resource_iterator = resource_cache.loaded_resources_.find(
        normalize_resource_path(resource_path));
    return resource_iterator == resource_cache.loaded_resources_.end()
        ? ResourceHandle{}
        : resource_iterator->second;
}

// Native: Resource_Load() - VA 0x004B5F00 - partial adapter, not a complete literal port.
// Behavior: Loads cached/unpacked bytes and optionally writes their uint32 byte count.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_87092f5147addf2b19ea080dbc70a44d1218ec706e022f019e4ba19bad366057.
// CFG: r2 blocks=10, edges=12; BN blocks=10; source agreement=100.0%.
// Confidence: partial host adapter; retail allocator/VFS/registry behavior is incomplete.
// Signature: C++ ResourceHandle/ResourceArchive signatures adapt the retail raw-pointer API.
// Usage/tests: Used by model/QVM loading; resource fixtures and model-scene runtime passed.
ResourceHandle Host_LoadResourceFile(
    ResourceCache& resource_cache,
    const std::filesystem::path& resource_path,
    std::uint32_t* out_byte_count)
{
    const std::string normalized_path = normalize_resource_path(resource_path);
    const auto cached_resource_iterator = resource_cache.loaded_resources_.find(normalized_path);
    if (cached_resource_iterator != resource_cache.loaded_resources_.end()) {
        if (out_byte_count != nullptr) {
            *out_byte_count = static_cast<std::uint32_t>(cached_resource_iterator->second->size());
        }
        return cached_resource_iterator->second;
    }

    auto loaded_resource = std::make_shared<LoadedResource>();
    loaded_resource->resource_path = normalized_path;
    loaded_resource->bytes = read_resource_file(resource_path);

    const auto loaded_byte_count = static_cast<std::uint32_t>(loaded_resource->size());
    ResourceHandle resource_handle = std::move(loaded_resource);
    resource_cache.loaded_resources_.emplace(normalized_path, resource_handle);
    if (out_byte_count != nullptr) {
        *out_byte_count = loaded_byte_count;
    }
    return resource_handle;
}

// Host adapter: drops a path from the port cache; see the header note.
// Behavior: Finds a resource, conditionally releases supported entry types, and errors if absent.
// Tools: REA/Ghidra, Binary Ninja MCP, radare2; byte prefixes match the retail image.
// Evidence: ev_61eb49f6f63a80b807875b80fe9cebc7ec3cf1637b5215987b4858715812e9bd.
// CFG: r2 blocks=7, edges=9; BN blocks=7; source agreement=100.0%.
// Confidence: partial adapter; host false-on-missing differs from the native fatal-error path.
// Signature: Retail void/path API is adapted to bool/cache/path; native ABI is not preserved yet.
// Usage/tests: Used by resource lifecycle fixtures; handle lifetime passed; retail unload replay pending.
bool Host_UnloadResourceFile(ResourceCache& resource_cache, const std::filesystem::path& resource_path)
{
    const std::string normalized_path = normalize_resource_path(resource_path);
    return resource_cache.loaded_resources_.erase(normalized_path) != 0U;
}

} // namespace igi::retail::resource
