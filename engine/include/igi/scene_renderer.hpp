#pragma once

#include "igi/game_simulation.hpp"

#include <cstddef>
#include <memory>
#include <span>

namespace igi::gameplay
{

enum class TextureFilterMode
{
    nearest_mipmap,
    trilinear_mipmap
};

struct FreeCamera
{
    Vector3 position{};
    float heading{};
    float pitch{};
    bool active = false;
};

class SceneRenderer final
{
  public:
    SceneRenderer(int width,
                  int height,
                  std::size_t worker_count = 0,
                  TextureFilterMode texture_filter_mode = TextureFilterMode::nearest_mipmap,
                  std::uint32_t maximum_anisotropy = 1);
    void render(const MissionScene& scene,
                const GameSimulation& simulation,
                const FreeCamera* free_camera = nullptr);

    [[nodiscard]] std::span<const std::uint32_t> pixels() const noexcept
    {
        return pixels_;
    }

    [[nodiscard]] int width() const noexcept
    {
        return width_;
    }

    [[nodiscard]] int height() const noexcept
    {
        return height_;
    }

    [[nodiscard]] std::uint64_t visible_triangles() const noexcept
    {
        return visible_triangles_;
    }

    [[nodiscard]] std::size_t terrain_chunks_culled() const noexcept
    {
        return terrain_chunks_culled_;
    }

    [[nodiscard]] std::size_t terrain_triangles_submitted() const noexcept
    {
        return terrain_triangles_submitted_;
    }

    void set_detail(TerrainDetail detail) noexcept
    {
        detail_ = detail;
        for (auto& worker : render_workers_)
        {
            if (worker)
            {
                worker->set_detail(detail);
            }
        }
    }

    [[nodiscard]] TerrainDetail detail() const noexcept
    {
        return detail_;
    }

  private:
    void triangle(const SceneTriangle& triangle,
                  const std::vector<SceneTexture>& textures,
                  Vector3 camera,
                  Vector3 right,
                  Vector3 up,
                  Vector3 forward,
                  float focal_length,
                  bool weapon = false,
                  SceneFogParameters fog = {});
    void rasterize(const SceneTriangle& triangle,
                   const SceneTexture& texture,
                   const SceneTexture* lightmap_texture,
                   float focal_length,
                   float illumination,
                   bool weapon,
                   SceneFogParameters fog);
    void rectangle(int left, int top, int width, int height, std::uint32_t color);
    void text(int left, int top, std::string_view message, std::uint32_t color, int scale = 1);
    void render_scope_overlay(int center_x, int center_y, float zoom, bool hit_marker);
    int width_{};
    int height_{};
    std::vector<std::uint32_t> pixels_;
    std::vector<float> depth_;
    std::uint64_t visible_triangles_{};
    std::size_t terrain_chunks_culled_{};
    std::size_t terrain_triangles_submitted_{};
    int first_scanline_{};
    int last_scanline_{};
    std::vector<std::unique_ptr<SceneRenderer>> render_workers_;
    TextureFilterMode texture_filter_mode_;
    std::uint32_t maximum_anisotropy_{};
    TerrainDetail detail_{TerrainDetail::high};
};

} // namespace igi::gameplay
