#pragma once

#include "igi/mission_scene.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

struct TEXImage;

namespace igi::assets
{

[[nodiscard]] gameplay::SceneTexture decode_original_texture(const TEXImage& image);

/// <summary>One model LOD and its slot in an object's depth-first lightmap layout.</summary>
struct ModelLightmapEntry final
{
    std::string model_name;
    /// <summary>Zero-based retail OLM index, or -1 when the node claims no lightmap slots.</summary>
    std::int64_t lightmap_index{-1};

    /// <summary>Whether this LOD uses the lightmapped MEF format.</summary>
    bool lightmapped{};
    std::uint32_t lightmap_page_count{};
    std::size_t triangle_count{};
};

class MissionAssets final
{
  public:
    MissionAssets(const std::filesystem::path& game_directory,
                  const std::filesystem::path& mission_directory,
                  const std::filesystem::path& cache_directory);
    [[nodiscard]] std::shared_ptr<gameplay::SceneMesh> model(const std::string& name);
    [[nodiscard]] bool has_model(const std::string& name) const;
    [[nodiscard]] bool is_invisible_attachment(const std::string& name) const;
    [[nodiscard]] bool is_ladder_model(const std::string& name) const;
    [[nodiscard]] std::vector<std::uint32_t> terrain_textures(const std::filesystem::path& path);
    [[nodiscard]] std::vector<std::uint32_t> lightmap_textures(std::string_view set_name,
                                                               std::uint32_t model_index);
    /// <summary>Builds one model's retail LOD and recursive attachment lightmap layout.</summary>
    [[nodiscard]] std::vector<ModelLightmapEntry> model_lightmap_layout(const std::string& model_name);

    /// <summary>Builds several model trees with the shared index sequence used by retail BuildAll.</summary>
    [[nodiscard]] std::vector<ModelLightmapEntry>
    model_lightmap_layout(const std::vector<std::string>& model_names);

    [[nodiscard]] std::vector<gameplay::SceneTexture> take_textures()
    {
        return std::move(textures_);
    }

  private:
    struct LightmapArchiveEntry final
    {
        std::filesystem::path archive_path;
        std::string resource_name;
    };

    void load_archive(const std::filesystem::path& path, const std::filesystem::path& cache_directory);
    void load_mapping(const std::filesystem::path& path);
    void index_lightmap_archive(const std::filesystem::path& path);
    [[nodiscard]] std::uint32_t texture(const std::string& name);
    std::vector<gameplay::SceneTexture> textures_;
    std::unordered_map<std::string, std::filesystem::path> files_;
    std::unordered_map<std::string, std::vector<std::string>> material_names_;
    std::unordered_map<std::string, std::uint32_t> texture_indices_;
    std::unordered_map<std::string, LightmapArchiveEntry> lightmap_entries_;
    std::unordered_map<std::string, std::vector<std::uint32_t>> lightmap_texture_indices_;
    std::unordered_map<std::string, std::shared_ptr<gameplay::SceneMesh>> meshes_;
    std::unordered_set<std::string> loading_models_;
    std::unordered_map<std::string, std::string> magic_attachment_models_;
    std::unordered_set<std::string> invisible_attachment_models_;
    std::unordered_set<std::string> muzzle_flash_models_;
    std::unordered_set<std::string> ladder_models_;
};

} // namespace igi::assets
