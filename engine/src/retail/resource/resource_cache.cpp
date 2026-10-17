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

ResourceHandle Is_ResourceLoaded(
    const ResourceCache& resource_cache,
    const std::filesystem::path& resource_path)
{
    const auto resource_iterator = resource_cache.loaded_resources_.find(
        normalize_resource_path(resource_path));
    return resource_iterator == resource_cache.loaded_resources_.end()
        ? ResourceHandle{}
        : resource_iterator->second;
}

ResourceHandle Resource_Load(
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

bool Resource_Unload(
    ResourceCache& resource_cache,
    const std::filesystem::path& resource_path)
{
    const std::string normalized_path = normalize_resource_path(resource_path);
    return resource_cache.loaded_resources_.erase(normalized_path) != 0U;
}

} // namespace igi::retail::resource
