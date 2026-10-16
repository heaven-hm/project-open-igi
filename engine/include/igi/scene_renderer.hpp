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

  private:
    void triangle(const SceneTriangle& triangle,
                  const std::vector<SceneTexture>& textures,
                  Vector3 camera,
                  Vector3 right,
                  Vector3 up,
                  Vector3 forward,
                  float focal_length,
                  bool weapon = false);
    void rasterize(const SceneTriangle& triangle,
                   const SceneTexture& texture,
                   float focal_length,
                   float illumination,
                   bool weapon);
    void rectangle(int left, int top, int width, int height, std::uint32_t color);
    void text(int left, int top, std::string_view message, std::uint32_t color, int scale = 1);
    void render_scope_overlay(int center_x, int center_y, float zoom, bool hit_marker);
    int width_{};
    int height_{};
    std::vector<std::uint32_t> pixels_;
    std::vector<float> depth_;
    std::uint64_t visible_triangles_{};
    int first_scanline_{};
    int last_scanline_{};
    std::vector<std::unique_ptr<SceneRenderer>> render_workers_;
    TextureFilterMode texture_filter_mode_;
    std::uint32_t maximum_anisotropy_{};
};

} // namespace igi::gameplay
