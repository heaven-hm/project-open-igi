#include "igi/scene_renderer.hpp"

#include "igi/skeletal_animation.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <future>
#include <thread>
#include <stdexcept>

namespace igi::gameplay
{
namespace
{

std::uint32_t mix_color(std::uint32_t first, std::uint32_t second, float weight)
{
    std::uint32_t result = 0xff000000u;
    for (const auto shift : {0, 8, 16})
    {
        const float channel = static_cast<float>((first >> shift) & 255) * (1 - weight) +
                              static_cast<float>((second >> shift) & 255) * weight;
        result |= static_cast<std::uint32_t>(std::clamp(channel, 0.0f, 255.0f)) << shift;
    }
    return result;
}

std::uint32_t texture_sample_level(const SceneTexture& texture,
                                  std::size_t mip_level,
                                  float horizontal,
                                  float vertical)
{
    if (!std::isfinite(horizontal) || !std::isfinite(vertical))
    {
        return 0;
    }
    std::uint32_t width = texture.width;
    std::uint32_t height = texture.height;
    const std::vector<std::uint32_t>* pixels = &texture.pixels;
    if (mip_level > 0 && mip_level <= texture.mipmaps.size())
    {
        const auto& mipmap = texture.mipmaps[mip_level - 1];
        width = mipmap.width;
        height = mipmap.height;
        pixels = &mipmap.pixels;
    }
    if (width == 0 || height == 0 || pixels->size() < static_cast<std::size_t>(width) * height)
    {
        return 0;
    }
    horizontal = (horizontal - std::floor(horizontal)) * static_cast<float>(width) - 0.5f;
    vertical = (vertical - std::floor(vertical)) * static_cast<float>(height) - 0.5f;
    const int left = static_cast<int>(std::floor(horizontal));
    const int top = static_cast<int>(std::floor(vertical));
    const auto horizontal_weight = static_cast<std::uint32_t>((horizontal - static_cast<float>(left)) * 256);
    const auto vertical_weight = static_cast<std::uint32_t>((vertical - static_cast<float>(top)) * 256);
    const int w = static_cast<int>(width);
    const int h = static_cast<int>(height);
    const auto sample = [&](int column, int row)
    {
        const auto wrapped_column =
            (width & (width - 1)) == 0
                ? static_cast<std::uint32_t>(column) & (width - 1)
                : static_cast<std::uint32_t>((column % w + w) % w);
        const auto wrapped_row =
            (height & (height - 1)) == 0
                ? static_cast<std::uint32_t>(row) & (height - 1)
                : static_cast<std::uint32_t>((row % h + h) % h);
        return (*pixels)[static_cast<std::size_t>(wrapped_row) * width + wrapped_column];
    };
    const std::array<std::uint32_t, 4> samples{
        sample(left, top), sample(left + 1, top), sample(left, top + 1), sample(left + 1, top + 1)};
    std::uint32_t color = 0;
    for (const int shift : {0, 8, 16, 24})
    {
        const auto lower = ((samples[0] >> shift) & 255) * (256 - horizontal_weight) +
                           ((samples[1] >> shift) & 255) * horizontal_weight;
        const auto upper = ((samples[2] >> shift) & 255) * (256 - horizontal_weight) +
                           ((samples[3] >> shift) & 255) * horizontal_weight;
        color |= ((lower * (256 - vertical_weight) + upper * vertical_weight) >> 16) << shift;
    }
    return color;
}

std::uint32_t texture_sample(const SceneTexture& texture,
                             float horizontal,
                             float vertical,
                             float mip_level,
                             TextureFilterMode filter_mode,
                             std::size_t usable_mipmap_count,
                             float horizontal_u_derivative,
                             float horizontal_v_derivative,
                             float vertical_u_derivative,
                             float vertical_v_derivative,
                             std::uint32_t maximum_anisotropy)
{
    if (usable_mipmap_count == 0 || !std::isfinite(mip_level) || mip_level <= 0.0f)
    {
        return texture_sample_level(texture, 0, horizontal, vertical);
    }
    const std::size_t available_level_count = usable_mipmap_count + 1;
    const float bounded_level = std::clamp(mip_level, 0.0f, static_cast<float>(usable_mipmap_count));
    const auto lower_index = static_cast<std::size_t>(std::floor(bounded_level));
    if (filter_mode == TextureFilterMode::nearest_mipmap)
    {
        const auto nearest_index = static_cast<std::size_t>(std::floor(bounded_level + 0.5f));
        return texture_sample_level(texture, nearest_index, horizontal, vertical);
    }

    if (maximum_anisotropy > 1)
    {
        const float horizontal_texture_width = horizontal_u_derivative * static_cast<float>(texture.width);
        const float horizontal_texture_height = horizontal_v_derivative * static_cast<float>(texture.height);
        const float vertical_texture_width = vertical_u_derivative * static_cast<float>(texture.width);
        const float vertical_texture_height = vertical_v_derivative * static_cast<float>(texture.height);
        const float horizontal_footprint_squared = horizontal_texture_width * horizontal_texture_width +
                                                   horizontal_texture_height * horizontal_texture_height;
        const float vertical_footprint_squared = vertical_texture_width * vertical_texture_width +
                                                 vertical_texture_height * vertical_texture_height;
        const bool horizontal_is_major = horizontal_footprint_squared >= vertical_footprint_squared;
        const float major_footprint = std::sqrt(std::max(horizontal_footprint_squared,
                                                         vertical_footprint_squared));
        const float minor_footprint = std::sqrt(std::max(0.000001f,
                                                         std::min(horizontal_footprint_squared,
                                                                  vertical_footprint_squared)));
        const auto requested_tap_count = static_cast<std::uint32_t>(
            std::clamp(std::ceil(major_footprint / minor_footprint), 1.0f, 16.0f));
        const std::uint32_t tap_count = std::min(requested_tap_count, maximum_anisotropy);
        if (tap_count > 1)
        {
            const float anisotropic_mip_level = minor_footprint > 1.0f ? std::log2(minor_footprint) : 0.0f;
            const float bounded_anisotropic_mip_level =
                std::clamp(anisotropic_mip_level, 0.0f, static_cast<float>(usable_mipmap_count));
            const auto anisotropic_lower_index =
                static_cast<std::size_t>(std::floor(bounded_anisotropic_mip_level));
            const auto anisotropic_upper_index =
                std::min(anisotropic_lower_index + 1, available_level_count - 1);
            const float anisotropic_mip_blend =
                bounded_anisotropic_mip_level - static_cast<float>(anisotropic_lower_index);
            std::array<std::uint32_t, 16> anisotropic_samples{};
            for (std::uint32_t tap_index = 0; tap_index < tap_count; ++tap_index)
            {
                const float tap_offset = (static_cast<float>(tap_index) + 0.5f) /
                                             static_cast<float>(tap_count) -
                                         0.5f;
                const float sample_u = horizontal +
                    (horizontal_is_major ? horizontal_u_derivative : vertical_u_derivative) * tap_offset;
                const float sample_v = vertical +
                    (horizontal_is_major ? horizontal_v_derivative : vertical_v_derivative) * tap_offset;
                const auto lower_color =
                    texture_sample_level(texture, anisotropic_lower_index, sample_u, sample_v);
                const auto upper_color =
                    texture_sample_level(texture, anisotropic_upper_index, sample_u, sample_v);
                anisotropic_samples[tap_index] = mix_color(lower_color, upper_color, anisotropic_mip_blend);
            }
            std::uint32_t averaged_color = 0;
            for (const std::uint32_t channel_shift : {0u, 8u, 16u, 24u})
            {
                std::uint32_t channel_total = 0;
                for (std::uint32_t tap_index = 0; tap_index < tap_count; ++tap_index)
                {
                    channel_total += (anisotropic_samples[tap_index] >> channel_shift) & 255u;
                }
                averaged_color |= (channel_total / tap_count) << channel_shift;
            }
            return averaged_color;
        }
    }

    const auto upper_index = std::min(lower_index + 1, available_level_count - 1);
    const auto lower_color = texture_sample_level(texture, lower_index, horizontal, vertical);
    const auto upper_color = texture_sample_level(texture, upper_index, horizontal, vertical);
    return mix_color(lower_color, upper_color, bounded_level - static_cast<float>(lower_index));
}

bool terrain_chunk_intersects_view(const TerrainRenderChunk& terrain_chunk,
                                   Vector3 camera,
                                   Vector3 right,
                                   Vector3 up,
                                   Vector3 forward,
                                   float horizontal_half_view_ratio,
                                   float vertical_half_view_ratio) noexcept
{
    const Vector3 camera_relative_center = terrain_chunk.bounds_center - camera;
    const float horizontal_position = dot(camera_relative_center, right);
    const float vertical_position = dot(camera_relative_center, up);
    const float forward_position = dot(camera_relative_center, forward);
    const float furthest_forward_position = forward_position + terrain_chunk.bounds_radius;
    const float nearest_forward_position = forward_position - terrain_chunk.bounds_radius;

    if (furthest_forward_position < 0.05f || nearest_forward_position > 1400.0f)
    {
        return false;
    }

    const float conservative_forward_extent = std::max(0.05f, furthest_forward_position);
    const float horizontal_extent =
        conservative_forward_extent * horizontal_half_view_ratio + terrain_chunk.bounds_radius;
    const float vertical_extent =
        conservative_forward_extent * vertical_half_view_ratio + terrain_chunk.bounds_radius;
    return std::abs(horizontal_position) <= horizontal_extent && std::abs(vertical_position) <= vertical_extent;
}

} // namespace

SceneRenderer::SceneRenderer(int width,
                             int height,
                             std::size_t worker_count,
                             TextureFilterMode texture_filter_mode,
                             std::uint32_t maximum_anisotropy)
    : width_(width),
      height_(height),
      texture_filter_mode_(texture_filter_mode),
      maximum_anisotropy_(maximum_anisotropy)
{
    if (width < 160 || height < 120 || width > 3840 || height > 2160)
    {
        throw std::runtime_error("Render dimensions are outside the supported range");
    }
    pixels_.resize(static_cast<std::size_t>(width) * height);
    depth_.resize(pixels_.size());
    last_scanline_ = height_;
    if (worker_count == 0)
    {
        worker_count = std::clamp(std::thread::hardware_concurrency(), 1u, 4u);
    }
    if (worker_count > 16)
    {
        throw std::runtime_error("Renderer worker count exceeds the supported limit");
    }
    if (maximum_anisotropy_ == 0 || maximum_anisotropy_ > 16 ||
        (maximum_anisotropy_ & (maximum_anisotropy_ - 1)) != 0)
    {
        throw std::runtime_error("Texture anisotropy must be one of 1, 2, 4, 8, or 16");
    }
    if (worker_count > 1)
    {
        render_workers_.reserve(worker_count);
        for (std::size_t worker_index = 0; worker_index < worker_count; ++worker_index)
        {
            auto worker = std::make_unique<SceneRenderer>(
                width_, height_, 1, texture_filter_mode_, maximum_anisotropy_);
            worker->first_scanline_ = static_cast<int>(worker_index * height_ / worker_count);
            worker->last_scanline_ = static_cast<int>((worker_index + 1) * height_ / worker_count);
            render_workers_.push_back(std::move(worker));
        }
    }
}

void SceneRenderer::triangle(const SceneTriangle& original,
                             const std::vector<SceneTexture>& textures,
                             Vector3 camera,
                             Vector3 right,
                             Vector3 up,
                             Vector3 forward,
                             float focal_length,
                             bool weapon,
                             SceneFogParameters fog)
{
    SceneTriangle transformed = original;
    for (auto& vertex : transformed.vertices)
    {
        const auto relative = vertex.position - camera;
        vertex.position = {dot(relative, right), dot(relative, up), dot(relative, forward)};
    }
    const auto& vertices = transformed.vertices;
    const float max_depth = detail_ == TerrainDetail::ultra ? 2500.0f
                          : detail_ == TerrainDetail::enhanced ? 1800.0f
                          : 1400.0f;
    if (std::max({vertices[0].position.z, vertices[1].position.z, vertices[2].position.z}) < 0.05f ||
        std::min({vertices[0].position.z, vertices[1].position.z, vertices[2].position.z}) > max_depth)
    {
        return;
    }
    const auto normal = normalized(cross(original.vertices[1].position - original.vertices[0].position,
                                         original.vertices[2].position - original.vertices[0].position));
    const float illumination =
        weapon ? 0.95f
               : 0.62f + 0.35f * std::clamp(dot(normal, normalized(Vector3{-0.35f, -0.45f, 0.82f})), 0.0f, 1.0f) +
                 0.08f * std::clamp(normal.z, 0.0f, 1.0f);
    std::array<SceneVertex, 5> clipped;
    std::size_t vertex_count = 0;
    for (std::size_t corner = 0; corner < 3; ++corner)
    {
        const auto& current = vertices[corner];
        const auto& previous = vertices[(corner + 2) % 3];
        const bool current_inside = current.position.z >= 0.05f;
        const bool previous_inside = previous.position.z >= 0.05f;
        if (current_inside != previous_inside)
        {
            const float fraction = (0.05f - previous.position.z) / (current.position.z - previous.position.z);
            SceneVertex intersection = previous;
            intersection.position = previous.position + (current.position - previous.position) * fraction;
            intersection.texture_u = previous.texture_u + (current.texture_u - previous.texture_u) * fraction;
            intersection.texture_v = previous.texture_v + (current.texture_v - previous.texture_v) * fraction;
            intersection.lightmap_u =
                previous.lightmap_u + (current.lightmap_u - previous.lightmap_u) * fraction;
            intersection.lightmap_v =
                previous.lightmap_v + (current.lightmap_v - previous.lightmap_v) * fraction;
            clipped[vertex_count++] = intersection;
        }
        if (current_inside)
        {
            clipped[vertex_count++] = current;
        }
    }
    if (original.texture_index >= textures.size())
    {
        throw std::runtime_error("Render triangle references an unavailable texture");
    }
    if (original.lightmap_texture_index < -1)
    {
        throw std::runtime_error("Render triangle contains an invalid negative lightmap texture index");
    }
    const SceneTexture* lightmap_texture = nullptr;
    if (original.lightmap_texture_index >= 0)
    {
        const auto lightmap_texture_index = static_cast<std::size_t>(original.lightmap_texture_index);
        if (lightmap_texture_index >= textures.size())
        {
            throw std::runtime_error("Render triangle references an unavailable lightmap texture");
        }
        lightmap_texture = &textures[lightmap_texture_index];
    }
    for (std::size_t corner = 1; corner + 1 < vertex_count; ++corner)
    {
        SceneTriangle clipped_triangle{{clipped[0], clipped[corner], clipped[corner + 1]},
                                       original.texture_index};
        clipped_triangle.firing_only = original.firing_only;
        clipped_triangle.opacity = original.opacity;
        clipped_triangle.lightmap_page_index = original.lightmap_page_index;
        clipped_triangle.lightmap_texture_index = original.lightmap_texture_index;
        rasterize(
            clipped_triangle,
            textures[original.texture_index],
            lightmap_texture,
            focal_length,
            illumination,
            weapon,
            fog);
    }
}

void SceneRenderer::rasterize(const SceneTriangle& triangle,
                              const SceneTexture& texture,
                              const SceneTexture* lightmap_texture,
                              float focal_length,
                              float illumination,
                              bool weapon,
                              SceneFogParameters fog)
{
    struct Projected final
    {
        float horizontal;
        float vertical;
        float inverse_depth;
        float texture_u;
        float texture_v;
        float lightmap_u;
        float lightmap_v;
        float fog_factor;
    };

    std::array<Projected, 3> projected;
    for (std::size_t corner = 0; corner < 3; ++corner)
    {
        const auto& vertex = triangle.vertices[corner];
        const float inverse = 1.0f / vertex.position.z;
        projected[corner] = {
            width_ * 0.5f + vertex.position.x * inverse * focal_length,
            height_ * 0.5f - vertex.position.y * inverse * focal_length,
            inverse,
            vertex.texture_u * inverse,
            vertex.texture_v * inverse,
            vertex.lightmap_u * inverse,
            vertex.lightmap_v * inverse,
            fog.density_per_metre > 0.0f ? std::exp(-fog.density_per_metre * vertex.position.z) : 1.0f};
    }
    const auto& first = projected[0];
    const auto& second = projected[1];
    const auto& third = projected[2];
    const float area = (second.horizontal - first.horizontal) * (third.vertical - first.vertical) -
                       (second.vertical - first.vertical) * (third.horizontal - first.horizontal);
    if (!std::isfinite(area) || std::abs(area) < 0.001f)
    {
        return;
    }
    const float minimum_horizontal = std::min({first.horizontal, second.horizontal, third.horizontal});
    const float maximum_horizontal = std::max({first.horizontal, second.horizontal, third.horizontal});
    const float minimum_vertical = std::min({first.vertical, second.vertical, third.vertical});
    const float maximum_vertical = std::max({first.vertical, second.vertical, third.vertical});
    if (minimum_horizontal >= width_ || maximum_horizontal < 0 || minimum_vertical >= height_ ||
        maximum_vertical < 0)
    {
        return;
    }
    const int left = static_cast<int>(std::clamp(minimum_horizontal, 0.0f, static_cast<float>(width_ - 1)));
    const int right = static_cast<int>(std::clamp(maximum_horizontal, 0.0f, static_cast<float>(width_ - 1)));
    const int top = std::max(first_scanline_,
        static_cast<int>(std::clamp(minimum_vertical, 0.0f, static_cast<float>(height_ - 1))));
    const int bottom = std::min(last_scanline_ - 1,
        static_cast<int>(std::clamp(maximum_vertical, 0.0f, static_cast<float>(height_ - 1))));
    const float first_step = (second.vertical - third.vertical) / area;
    const float second_step = (third.vertical - first.vertical) / area;
    const float third_step = (first.vertical - second.vertical) / area;
    const float first_row_step = (third.horizontal - second.horizontal) / area;
    const float second_row_step = (first.horizontal - third.horizontal) / area;
    const float third_row_step = (second.horizontal - first.horizontal) / area;
    const float inverse_depth_horizontal_step = first_step * first.inverse_depth +
                                                second_step * second.inverse_depth +
                                                third_step * third.inverse_depth;
    const float inverse_depth_vertical_step = first_row_step * first.inverse_depth +
                                              second_row_step * second.inverse_depth +
                                              third_row_step * third.inverse_depth;
    const float texture_u_horizontal_step = first_step * first.texture_u + second_step * second.texture_u +
                                            third_step * third.texture_u;
    const float texture_u_vertical_step = first_row_step * first.texture_u +
                                          second_row_step * second.texture_u +
                                          third_row_step * third.texture_u;
    const float texture_v_horizontal_step = first_step * first.texture_v + second_step * second.texture_v +
                                            third_step * third.texture_v;
    const float texture_v_vertical_step = first_row_step * first.texture_v +
                                          second_row_step * second.texture_v +
                                          third_row_step * third.texture_v;
    std::size_t usable_mipmap_count = 0;
    for (const auto& mipmap : texture.mipmaps)
    {
        if (texture_filter_mode_ == TextureFilterMode::nearest_mipmap && !mipmap.authored)
        {
            break;
        }
        ++usable_mipmap_count;
    }
    for (int row = top; row <= bottom; ++row)
    {
        const float horizontal = static_cast<float>(left) + 0.5f;
        const float vertical = static_cast<float>(row) + 0.5f;
        float first_weight = ((second.horizontal - horizontal) * (third.vertical - vertical) -
                              (second.vertical - vertical) * (third.horizontal - horizontal)) /
                             area;
        float second_weight = ((third.horizontal - horizontal) * (first.vertical - vertical) -
                               (third.vertical - vertical) * (first.horizontal - horizontal)) /
                              area;
        float third_weight = 1.0f - first_weight - second_weight;
        float first_column_offset = 0;
        float last_column_offset = static_cast<float>(right - left);
        const auto clip_span = [&](float weight, float step)
        {
            if (step > 0.000000001f)
            {
                first_column_offset = std::max(first_column_offset, (-0.00001f - weight) / step);
            }
            else if (step < -0.000000001f)
            {
                last_column_offset = std::min(last_column_offset, (-0.00001f - weight) / step);
            }
            else if (weight < -0.00001f)
            {
                last_column_offset = -1;
            }
        };
        clip_span(first_weight, first_step);
        clip_span(second_weight, second_step);
        clip_span(third_weight, third_step);
        if (first_column_offset > last_column_offset)
        {
            continue;
        }
        const int span_start = std::max(0, static_cast<int>(std::ceil(first_column_offset)));
        const int span_end = std::min(right - left, static_cast<int>(std::floor(last_column_offset)));
        first_weight += first_step * static_cast<float>(span_start);
        second_weight += second_step * static_cast<float>(span_start);
        third_weight += third_step * static_cast<float>(span_start);
        for (int column = left + span_start; column <= left + span_end; ++column)
        {
            const std::size_t pixel_index = static_cast<std::size_t>(row) * width_ + column;
            const float inverse_depth = first_weight * first.inverse_depth +
                                        second_weight * second.inverse_depth +
                                        third_weight * third.inverse_depth;
            if (first_weight >= -0.00001f && second_weight >= -0.00001f && third_weight >= -0.00001f &&
                inverse_depth > depth_[pixel_index])
            {
                const float pixel_depth = 1.0f / inverse_depth;
                const float texture_u = (first_weight * first.texture_u + second_weight * second.texture_u +
                                         third_weight * third.texture_u) *
                                        pixel_depth;
                const float texture_v = (first_weight * first.texture_v + second_weight * second.texture_v +
                                         third_weight * third.texture_v) *
                                        pixel_depth;
                const float lightmap_u =
                    (first_weight * first.lightmap_u + second_weight * second.lightmap_u +
                     third_weight * third.lightmap_u) *
                    pixel_depth;
                const float lightmap_v =
                    (first_weight * first.lightmap_v + second_weight * second.lightmap_v +
                     third_weight * third.lightmap_v) *
                    pixel_depth;
                float mip_level = 0.0f;
                float texture_u_horizontal_derivative = 0.0f;
                float texture_u_vertical_derivative = 0.0f;
                float texture_v_horizontal_derivative = 0.0f;
                float texture_v_vertical_derivative = 0.0f;
                if (usable_mipmap_count > 0)
                {
                    texture_u_horizontal_derivative =
                        pixel_depth * (texture_u_horizontal_step - texture_u * inverse_depth_horizontal_step);
                    texture_u_vertical_derivative =
                        pixel_depth * (texture_u_vertical_step - texture_u * inverse_depth_vertical_step);
                    texture_v_horizontal_derivative =
                        pixel_depth * (texture_v_horizontal_step - texture_v * inverse_depth_horizontal_step);
                    texture_v_vertical_derivative =
                        pixel_depth * (texture_v_vertical_step - texture_v * inverse_depth_vertical_step);
                    const float horizontal_texture_width =
                        texture_u_horizontal_derivative * static_cast<float>(texture.width);
                    const float horizontal_texture_height =
                        texture_v_horizontal_derivative * static_cast<float>(texture.height);
                    const float vertical_texture_width =
                        texture_u_vertical_derivative * static_cast<float>(texture.width);
                    const float vertical_texture_height =
                        texture_v_vertical_derivative * static_cast<float>(texture.height);
                    const float horizontal_footprint_squared =
                        horizontal_texture_width * horizontal_texture_width +
                        horizontal_texture_height * horizontal_texture_height;
                    const float vertical_footprint_squared =
                        vertical_texture_width * vertical_texture_width +
                        vertical_texture_height * vertical_texture_height;
                    const float maximum_footprint_squared =
                        std::max(horizontal_footprint_squared, vertical_footprint_squared);
                    if (maximum_footprint_squared > 1.0f)
                    {
                        mip_level = 0.5f * std::log2(maximum_footprint_squared);
                    }
                }
                const auto sampled = texture_sample(texture,
                                                    texture_u,
                                                    texture_v,
                                                    mip_level,
                                                    texture_filter_mode_,
                                                    usable_mipmap_count,
                                                    texture_u_horizontal_derivative,
                                                    texture_v_horizontal_derivative,
                                                    texture_u_vertical_derivative,
                                                    texture_v_vertical_derivative,
                                                    maximum_anisotropy_);
                std::uint32_t sampled_surface = sampled;
                if (lightmap_texture != nullptr)
                {
                    const std::uint32_t sampled_lightmap =
                        texture_sample_level(*lightmap_texture, 0, lightmap_u, lightmap_v);
                    sampled_surface = sampled & 0xff000000u;
                    for (const int channel_shift : {0, 8, 16})
                    {
                        const std::uint32_t diffuse_channel = (sampled >> channel_shift) & 255u;
                        const std::uint32_t lightmap_channel = (sampled_lightmap >> channel_shift) & 255u;
                        sampled_surface |= ((diffuse_channel * lightmap_channel + 127u) / 255u)
                                           << channel_shift;
                    }
                }
                if (triangle.firing_only)
                {
                    const std::uint32_t r = (sampled_surface >> 16) & 255;
                    const std::uint32_t g = (sampled_surface >> 8) & 255;
                    const std::uint32_t b = sampled_surface & 255;
                    const std::uint32_t a = (sampled_surface >> 24) & 255;
                    if (r + g + b < 30 || a < 32)
                    {
                        continue;
                    }
                    const auto bg = pixels_[pixel_index];
                    const std::uint32_t bg_r = (bg >> 16) & 255;
                    const std::uint32_t bg_g = (bg >> 8) & 255;
                    const std::uint32_t bg_b = bg & 255;
                    const std::uint32_t out_r = std::min(255u, bg_r + r);
                    const std::uint32_t out_g = std::min(255u, bg_g + g);
                    const std::uint32_t out_b = std::min(255u, bg_b + b);
                    pixels_[pixel_index] = 0xff000000u | (out_r << 16) | (out_g << 8) | out_b;
                    continue;
                }
                if ((sampled_surface >> 24) >= 96)
                {
                    std::uint32_t shaded = 0xff000000u;
                    for (const auto shift : {0, 8, 16})
                    {
                        shaded |= static_cast<std::uint32_t>(static_cast<float>((sampled_surface >> shift) & 255) *
                                                             illumination)
                                  << shift;
                    }
                    const float fog_survival = first_weight * first.fog_factor +
                                               second_weight * second.fog_factor +
                                               third_weight * third.fog_factor;
                    const float fog_opacity = weapon ? 0.0f : 1.0f - std::clamp(fog_survival, 0.0f, 1.0f);
                    const auto red =
                        static_cast<std::uint32_t>(std::clamp(fog.colour.x, 0.0f, 1.0f) * 255.0f);
                    const auto green =
                        static_cast<std::uint32_t>(std::clamp(fog.colour.y, 0.0f, 1.0f) * 255.0f);
                    const auto blue =
                        static_cast<std::uint32_t>(std::clamp(fog.colour.z, 0.0f, 1.0f) * 255.0f);
                    const std::uint32_t fog_colour = 0xff000000u | (red << 16) | (green << 8) | blue;
                    const std::uint32_t fogged_surface = mix_color(shaded, fog_colour, fog_opacity);
                    if (triangle.opacity < 1.0f)
                    {
                        pixels_[pixel_index] = mix_color(pixels_[pixel_index],
                                                         fogged_surface,
                                                         std::clamp(triangle.opacity, 0.0f, 1.0f));
                    }
                    else
                    {
                        pixels_[pixel_index] = fogged_surface;
                    }
                    depth_[pixel_index] = inverse_depth;
                }
            }
            first_weight += first_step;
            second_weight += second_step;
            third_weight += third_step;
        }
    }
    ++visible_triangles_;
}

void SceneRenderer::rectangle(int left, int top, int width, int height, std::uint32_t color)
{
    for (int row = std::max(0, top); row < std::min(height_, top + height); ++row)
    {
        for (int column = std::max(0, left); column < std::min(width_, left + width); ++column)
        {
            pixels_[static_cast<std::size_t>(row) * width_ + column] = color;
        }
    }
}

void SceneRenderer::text(int left, int top, std::string_view message, std::uint32_t color, int scale)
{
    static constexpr std::array<std::array<std::uint8_t, 7>, 36> glyphs{
        {{{14, 17, 19, 21, 25, 17, 14}}, {{4, 12, 4, 4, 4, 4, 14}},      {{14, 17, 1, 2, 4, 8, 31}},
         {{30, 1, 1, 14, 1, 1, 30}},     {{2, 6, 10, 18, 31, 2, 2}},     {{31, 16, 16, 30, 1, 1, 30}},
         {{14, 16, 16, 30, 17, 17, 14}}, {{31, 1, 2, 4, 8, 8, 8}},       {{14, 17, 17, 14, 17, 17, 14}},
         {{14, 17, 17, 15, 1, 1, 14}},   {{14, 17, 17, 31, 17, 17, 17}}, {{30, 17, 17, 30, 17, 17, 30}},
         {{14, 17, 16, 16, 16, 17, 14}}, {{30, 17, 17, 17, 17, 17, 30}}, {{31, 16, 16, 30, 16, 16, 31}},
         {{31, 16, 16, 30, 16, 16, 16}}, {{14, 17, 16, 23, 17, 17, 15}}, {{17, 17, 17, 31, 17, 17, 17}},
         {{14, 4, 4, 4, 4, 4, 14}},      {{7, 2, 2, 2, 2, 18, 12}},      {{17, 18, 20, 24, 20, 18, 17}},
         {{16, 16, 16, 16, 16, 16, 31}}, {{17, 27, 21, 21, 17, 17, 17}}, {{17, 25, 21, 19, 17, 17, 17}},
         {{14, 17, 17, 17, 17, 17, 14}}, {{30, 17, 17, 30, 16, 16, 16}}, {{14, 17, 17, 17, 21, 18, 13}},
         {{30, 17, 17, 30, 20, 18, 17}}, {{15, 16, 16, 14, 1, 1, 30}},   {{31, 4, 4, 4, 4, 4, 4}},
         {{17, 17, 17, 17, 17, 17, 14}}, {{17, 17, 17, 17, 17, 10, 4}},  {{17, 17, 17, 21, 21, 21, 10}},
         {{17, 17, 10, 4, 10, 17, 17}},  {{17, 17, 10, 4, 4, 4, 4}},     {{31, 1, 2, 4, 8, 16, 31}}}};
    for (const auto character : message)
    {
        const auto uppercase = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
        const int glyph_index = uppercase >= '0' && uppercase <= '9'   ? uppercase - '0'
                                : uppercase >= 'A' && uppercase <= 'Z' ? uppercase - 'A' + 10
                                                                       : -1;
        if (glyph_index >= 0)
        {
            for (int row = 0; row < 7; ++row)
            {
                for (int column = 0; column < 5; ++column)
                {
                    if ((glyphs[glyph_index][row] & (1 << (4 - column))) != 0)
                    {
                        rectangle(left + column * scale, top + row * scale, scale, scale, color);
                    }
                }
            }
        }
        else if (character == '/')
        {
            for (int row = 0; row < 7; ++row)
            {
                rectangle(left + (4 - row * 4 / 6) * scale, top + row * scale, scale, scale, color);
            }
        }
        left += 6 * scale;
    }
}

void SceneRenderer::render_scope_overlay(int center_x, int center_y, float zoom, bool hit_marker)
{
    const int radius = static_cast<int>(height_ * 0.44f);
    const int radius_sq = radius * radius;
    const std::uint32_t mask_color = 0xff080b08u;
    const std::uint32_t ring_color = 0xff252c25u;
    const std::uint32_t reticle_color = hit_marker ? 0xffff4433u : 0xff141a14u;
    const std::uint32_t glow_color = hit_marker ? 0xffff7766u : 0xff3b4a3bu;

    for (int y = 0; y < height_; ++y)
    {
        const int dy = y - center_y;
        const int dy_sq = dy * dy;
        if (dy_sq >= radius_sq)
        {
            std::fill_n(pixels_.begin() + static_cast<std::size_t>(y) * width_, width_, mask_color);
        }
        else
        {
            const int dx = static_cast<int>(std::sqrt(static_cast<float>(radius_sq - dy_sq)));
            const int left_bound = std::max(0, center_x - dx);
            const int right_bound = std::min(width_, center_x + dx);
            if (left_bound > 0)
            {
                std::fill_n(pixels_.begin() + static_cast<std::size_t>(y) * width_, left_bound, mask_color);
            }
            if (right_bound < width_)
            {
                std::fill_n(pixels_.begin() + static_cast<std::size_t>(y) * width_ + right_bound, width_ - right_bound, mask_color);
            }
            if (left_bound < right_bound)
            {
                pixels_[static_cast<std::size_t>(y) * width_ + left_bound] = ring_color;
                if (left_bound + 1 < right_bound)
                {
                    pixels_[static_cast<std::size_t>(y) * width_ + left_bound + 1] = ring_color;
                }
                pixels_[static_cast<std::size_t>(y) * width_ + right_bound - 1] = ring_color;
                if (right_bound - 2 >= left_bound)
                {
                    pixels_[static_cast<std::size_t>(y) * width_ + right_bound - 2] = ring_color;
                }
            }
        }
    }

    const int chevron_height = 10;
    for (int i = 0; i <= chevron_height; ++i)
    {
        rectangle(center_x - i, center_y + i, 1, 2, reticle_color);
        rectangle(center_x + i, center_y + i, 1, 2, reticle_color);
    }
    const std::array<int, 3> sub_chevrons = {28, 56, 84};
    for (int offset_y : sub_chevrons)
    {
        const int cy = center_y + offset_y;
        if (cy + 7 < center_y + radius - 20)
        {
            for (int i = 0; i <= 6; ++i)
            {
                rectangle(center_x - i, cy + i, 1, 1, glow_color);
                rectangle(center_x + i, cy + i, 1, 1, glow_color);
            }
        }
    }

    const int inner_gap = 18;
    const int outer_extent = radius - 25;
    rectangle(center_x - outer_extent, center_y, outer_extent - inner_gap, 2, reticle_color);
    rectangle(center_x + inner_gap, center_y, outer_extent - inner_gap, 2, reticle_color);
    rectangle(center_x, center_y - outer_extent, 2, outer_extent - inner_gap, reticle_color);

    for (int d = inner_gap + 12; d < outer_extent; d += 16)
    {
        const int tick_h = ((d - inner_gap) % 32 == 0) ? 10 : 6;
        rectangle(center_x - d, center_y - tick_h / 2, 1, tick_h, glow_color);
        rectangle(center_x + d, center_y - tick_h / 2, 1, tick_h, glow_color);
    }

    const int rf_base_y = center_y + static_cast<int>(radius * 0.45f);
    const int rf_start_x = center_x - static_cast<int>(radius * 0.65f);
    const int rf_width = static_cast<int>(radius * 0.45f);
    rectangle(rf_start_x, rf_base_y, rf_width, 2, glow_color);
    for (int rx = 0; rx < rf_width; ++rx)
    {
        const float t = static_cast<float>(rx) / static_cast<float>(rf_width);
        const int cur_h = static_cast<int>(7.0f + 19.0f * (1.0f - t) * (1.0f - t));
        rectangle(rf_start_x + rx, rf_base_y - cur_h, 1, 1, glow_color);
    }
    text(rf_start_x, rf_base_y + 4, "10", glow_color);
    text(rf_start_x + rf_width / 4, rf_base_y + 4, "8", glow_color);
    text(rf_start_x + rf_width / 2, rf_base_y + 4, "6", glow_color);
    text(rf_start_x + 3 * rf_width / 4, rf_base_y + 4, "4", glow_color);
    text(rf_start_x + rf_width - 8, rf_base_y + 4, "2", glow_color);
    text(rf_start_x, rf_base_y - 34, "1.7M", glow_color);

    const int zoom_int = static_cast<int>(zoom * 10.0f + 0.5f);
    const std::string zoom_str = std::to_string(zoom_int / 10) + "." + std::to_string(zoom_int % 10) + "X";
    rectangle(center_x - 70, center_y - radius + 15, 140, 20, 0xff17211bu);
    text(center_x - 55, center_y - radius + 21, "SVD " + zoom_str, 0xff9cba81u);
}

void SceneRenderer::render(const MissionScene& scene,
                           const GameSimulation& simulation,
                           const FreeCamera* free_camera)
{
    if (scene.textures.empty())
    {
        throw std::runtime_error("Scene has no texture data");
    }
    if (!render_workers_.empty())
    {
        std::vector<std::future<void>> pending_frames;
        pending_frames.reserve(render_workers_.size());
        for (const auto& worker : render_workers_)
        {
            pending_frames.push_back(std::async(std::launch::async,
                [&scene, &simulation, free_camera, renderer = worker.get()] {
                    renderer->render(scene, simulation, free_camera);
                }));
        }
        for (auto& pending_frame : pending_frames)
        {
            pending_frame.get();
        }
        for (const auto& worker : render_workers_)
        {
            const auto first_pixel = static_cast<std::ptrdiff_t>(worker->first_scanline_) * width_;
            const auto last_pixel = static_cast<std::ptrdiff_t>(worker->last_scanline_) * width_;
            std::copy(worker->pixels_.begin() + first_pixel, worker->pixels_.begin() + last_pixel,
                      pixels_.begin() + first_pixel);
        }
        visible_triangles_ = render_workers_.front()->visible_triangles_;
        terrain_chunks_culled_ = render_workers_.front()->terrain_chunks_culled_;
        terrain_triangles_submitted_ = render_workers_.front()->terrain_triangles_submitted_;
        return;
    }
    visible_triangles_ = 0;
    terrain_chunks_culled_ = 0;
    terrain_triangles_submitted_ = 0;
    if (scene.terrain_triangle_count > scene.static_triangles.size())
    {
        throw std::runtime_error("Terrain triangle count exceeds the static scene triangle array");
    }
    std::fill(depth_.begin() + static_cast<std::ptrdiff_t>(first_scanline_) * width_,
              depth_.begin() + static_cast<std::ptrdiff_t>(last_scanline_) * width_, 0.0f);
    for (int row = first_scanline_; row < last_scanline_; ++row)
    {
        const auto sky = detail_ == TerrainDetail::ultra
            ? mix_color(0xff688faeu, 0xffb5bcbeu, static_cast<float>(row) / height_)
            : (detail_ == TerrainDetail::enhanced
                ? mix_color(0xff7896acu, 0xffbebfb7u, static_cast<float>(row) / height_)
                : mix_color(0xff899dacu, 0xffc4c2afu, static_cast<float>(row) / height_));
        std::fill_n(pixels_.begin() + static_cast<std::ptrdiff_t>(row) * width_, width_, sky);
    }
    const bool is_free_camera = free_camera && free_camera->active;
    const auto camera = is_free_camera ? free_camera->position : simulation.camera_position();
    const float heading = is_free_camera ? free_camera->heading : simulation.view_heading();
    const float pitch = is_free_camera ? free_camera->pitch : simulation.view_pitch();
    const auto forward = is_free_camera
        ? Vector3{-std::sin(heading) * std::cos(pitch), std::cos(heading) * std::cos(pitch), std::sin(pitch)}
        : simulation.aim_direction();
    const Vector3 right{std::cos(heading), std::sin(heading), 0};
    const Vector3 up = normalized(cross(right, forward));
    const float zoom_factor = is_free_camera ? 1.0f : simulation.zoom_factor();
    const float focal_length = static_cast<float>(width_) * 0.55f * zoom_factor;
    const auto render_static_triangle = [&](std::size_t triangle_index)
    {
        const auto& surface = scene.static_triangles[triangle_index];
        if (surface.firing_only)
        {
            return;
        }
        const auto& fog =
            triangle_index < scene.terrain_triangle_count ? scene.terrain_fog : scene.object_fog;
        triangle(surface, scene.textures, camera, right, up, forward, focal_length, false, fog);
    };

    if (scene.terrain_render_chunks.empty())
    {
        for (std::size_t triangle_index = 0; triangle_index < scene.static_triangles.size(); ++triangle_index)
        {
            if (triangle_index < scene.terrain_triangle_count)
            {
                ++terrain_triangles_submitted_;
            }
            render_static_triangle(triangle_index);
        }
    }
    else
    {
        std::size_t next_chunk_triangle = 0;
        for (const TerrainRenderChunk& terrain_chunk : scene.terrain_render_chunks)
        {
            if (terrain_chunk.triangle_count == 0 || terrain_chunk.first_triangle != next_chunk_triangle ||
                terrain_chunk.triangle_count > scene.terrain_triangle_count - next_chunk_triangle ||
                !finite(terrain_chunk.bounds_center) || !std::isfinite(terrain_chunk.bounds_radius) ||
                terrain_chunk.bounds_radius < 0.0f)
            {
                throw std::runtime_error("Terrain render chunk metadata is invalid or incomplete");
            }
            next_chunk_triangle += terrain_chunk.triangle_count;
            if (!terrain_chunk_intersects_view(terrain_chunk,
                                               camera,
                                               right,
                                               up,
                                               forward,
                                               static_cast<float>(width_) / (2.0f * focal_length),
                                               static_cast<float>(height_) / (2.0f * focal_length)))
            {
                ++terrain_chunks_culled_;
                continue;
            }
            terrain_triangles_submitted_ += terrain_chunk.triangle_count;
            const std::size_t chunk_end_triangle = terrain_chunk.first_triangle + terrain_chunk.triangle_count;
            for (std::size_t triangle_index = terrain_chunk.first_triangle; triangle_index < chunk_end_triangle;
                 ++triangle_index)
            {
                render_static_triangle(triangle_index);
            }
        }
        if (next_chunk_triangle != scene.terrain_triangle_count)
        {
            throw std::runtime_error("Terrain render chunks do not cover every terrain triangle");
        }
        for (std::size_t triangle_index = scene.terrain_triangle_count;
             triangle_index < scene.static_triangles.size();
             ++triangle_index)
        {
            render_static_triangle(triangle_index);
        }
    }
    const float max_door_dist = detail_ == TerrainDetail::ultra ? 850.0f
                               : detail_ == TerrainDetail::enhanced ? 550.0f
                               : 350.0f;
    for (const auto& door : simulation.doors())
    {
        if (!door.mesh || length(door.position - camera) > max_door_dist)
        {
            continue;
        }
        Vector3 door_pos = door.position;
        Vector3 door_rot = door.rotation;
        if (door.open)
        {
            if (door.model_id.find("304") != std::string::npos)
            {
                door_pos = door_pos + rotate_euler(Vector3{3.6f, 0.0f, 0.0f}, door.rotation);
            }
            else
            {
                door_rot.z += 1.5707963f;
            }
        }
        for (const auto& tri : door.mesh->triangles)
        {
            SceneTriangle world_tri = tri;
            for (auto& vertex : world_tri.vertices)
            {
                vertex.position = rotate_euler(vertex.position, door_rot) + door_pos;
            }
            triangle(
                world_tri, scene.textures, camera, right, up, forward, focal_length, false, scene.object_fog);
        }
    }
    const float max_actor_dist = detail_ == TerrainDetail::ultra ? 1200.0f
                                : detail_ == TerrainDetail::enhanced ? 750.0f
                                : 450.0f;
    for (std::size_t actor_index = 0; actor_index < simulation.actors().size(); ++actor_index)
    {
        const auto& actor = simulation.actors()[actor_index];
        const auto& mesh = scene.actors[actor_index].mesh;
        if (!actor.active || !mesh || length(actor.position - camera) > max_actor_dist)
        {
            continue;
        }
        const float cosine = std::cos(actor.heading);
        const float sine = std::sin(actor.heading);
        const auto& animations = scene.actors[actor_index].animations;
        const auto pose =
            animations
                ? sample_bone_pose(*animations,
                                   actor.animation_identifier,
                                   actor.animation_time,
                                   false,
                                   actor.behavior == ActorBehavior::dead)
                : std::vector<glm::mat4>{};
        for (auto surface : mesh->triangles)
        {
            if (surface.firing_only && actor.muzzle_flash_remaining <= 0)
            {
                continue;
            }
            for (auto& vertex : surface.vertices)
            {
                auto position = skin_vertex(vertex, pose);
                if (actor.behavior == ActorBehavior::dead && pose.empty())
                {
                    position = {position.x, position.z, 0.1f - position.y};
                }
                vertex.position = {position.x * cosine - position.y * sine + actor.position.x,
                                   position.x * sine + position.y * cosine + actor.position.y,
                                   position.z + actor.position.z};
            }
            triangle(
                surface, scene.textures, camera, right, up, forward, focal_length, false, scene.object_fog);
        }
        if (actor.behavior != ActorBehavior::dead && pose.size() > 22)
        {
            const auto weapon =
                std::find_if(scene.weapons.begin(),
                             scene.weapons.end(),
                             [&](const auto& candidate)
                             { return candidate.identifier == scene.actors[actor_index].weapon_identifier; });
            if (weapon != scene.weapons.end() && weapon->mesh)
            {
                for (auto surface : weapon->mesh->triangles)
                {
                    if (surface.firing_only && actor.muzzle_flash_remaining <= 0)
                    {
                        continue;
                    }
                    for (auto& vertex : surface.vertices)
                    {
                        const auto local =
                            pose[22] * glm::vec4(vertex.position.x, vertex.position.y, vertex.position.z, 1);
                        vertex.position = {local.x * cosine - local.y * sine + actor.position.x,
                                           local.x * sine + local.y * cosine + actor.position.y,
                                           local.z + actor.position.z};
                    }
                    triangle(surface,
                             scene.textures,
                             camera,
                             right,
                             up,
                             forward,
                             focal_length,
                             false,
                             scene.object_fog);
                }
            }
        }
    }
    const auto equipped_mesh = simulation.weapon_mesh();
    if (equipped_mesh && simulation.health() > 0 && !is_free_camera && !simulation.is_scoped())
    {
        std::fill(depth_.begin(), depth_.end(), 0.0f);
        if (scene.first_person_mesh && scene.first_person_animations &&
            simulation.weapon().animation_identifier >= 0)
        {
            const auto pose = sample_bone_pose(*scene.first_person_animations,
                                               simulation.weapon().animation_identifier,
                                               simulation.weapon().animation_time,
                                               true);
            const auto offset = sample_animation_root(*scene.first_person_animations,
                                                      simulation.weapon().animation_identifier,
                                                      simulation.weapon().animation_time);
            const float view_focal_length = static_cast<float>(width_) / (2 * 0.57735026f);
            for (auto surface : scene.first_person_mesh->triangles)
            {
                for (auto& vertex : surface.vertices)
                {
                    const auto position = skin_vertex(vertex, pose) + offset;
                    vertex.position = {position.x, position.z, position.y};
                }
                triangle(
                    surface, scene.textures, {}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, view_focal_length, true);
            }
            if (pose.size() > 3)
            {
                for (auto surface : equipped_mesh->triangles)
                {
                    if (surface.firing_only && simulation.weapon().muzzle_flash_remaining <= 0)
                    {
                        continue;
                    }
                    for (auto& vertex : surface.vertices)
                    {
                        const auto local =
                            pose[3] * glm::vec4(vertex.position.x, vertex.position.y, vertex.position.z, 1);
                        const auto position = Vector3{local.x, local.y, local.z} + offset;
                        vertex.position = {position.x, position.z, position.y};
                    }
                    triangle(surface,
                             scene.textures,
                             {},
                             {1, 0, 0},
                             {0, 1, 0},
                             {0, 0, 1},
                             view_focal_length,
                             true);
                }
            }
        }
        else
        {
            const float reload_drop = simulation.weapon().reload_remaining > 0 ? 0.35f : 0;
            for (auto surface : equipped_mesh->triangles)
            {
                if (surface.firing_only && simulation.weapon().muzzle_flash_remaining <= 0)
                {
                    continue;
                }
                for (auto& vertex : surface.vertices)
                {
                    vertex.position = {vertex.position.x + (simulation.aiming() ? 0 : 0.22f),
                                       vertex.position.z - 0.31f - reload_drop,
                                       vertex.position.y + 0.55f};
                }
                triangle(surface,
                         scene.textures,
                         {},
                         {1, 0, 0},
                         {0, 1, 0},
                         {0, 0, 1},
                         static_cast<float>(width_) * 0.55f,
                         true);
            }
        }
    }
    const int center_x = width_ / 2;
    const int center_y = height_ / 2;
    if (!is_free_camera)
    {
        if (simulation.is_scoped())
        {
            render_scope_overlay(center_x, center_y, simulation.zoom_factor(), simulation.weapon().hit_marker_remaining > 0);
        }
        else
        {
            const auto crosshair_color =
                simulation.weapon().hit_marker_remaining > 0 ? 0xffffb171u : 0xffdce1d1u;
            rectangle(center_x - 9, center_y, 5, 1, crosshair_color);
            rectangle(center_x + 5, center_y, 5, 1, crosshair_color);
            rectangle(center_x, center_y - 9, 1, 5, crosshair_color);
            rectangle(center_x, center_y + 5, 1, 5, crosshair_color);
        }
    }
    rectangle(14, height_ - 55, 195, 40, 0xff17211bu);
    text(24, height_ - 46, "HEALTH", 0xffa8bd99u);
    const float health_fraction =
        std::clamp(simulation.health() / scene.player_parameters.maximum_health, 0.0f, 1.0f);
    rectangle(24, height_ - 29, 170, 7, 0xff384233u);
    rectangle(24, height_ - 29, static_cast<int>(170 * health_fraction), 7, 0xff9cba81u);
    rectangle(width_ - 210, height_ - 55, 196, 40, 0xff17211bu);
    text(width_ - 197, height_ - 46, simulation.weapon_parameters().name, 0xffa8bd99u);
    text(width_ - 197,
         height_ - 31,
         std::to_string(simulation.weapon().magazine) + " / " + std::to_string(simulation.weapon().reserve),
         0xffe5e7cfu,
         2);
    if (simulation.weapon().reload_remaining > 0 && !is_free_camera)
    {
        text(center_x - 30, center_y + 25, "RELOADING", 0xffdce1d1u);
    }
    text(18, 18, "LEVEL " + std::to_string(scene.mission_number), 0xffe5e7cfu, 2);
    text(18,
         40,
         "WEAPON " + std::to_string(simulation.selected_weapon_slot() + 1) + " / " +
             std::to_string(simulation.inventory_size()) + "   CONTROLS FROM CONFIG",
         0xffe5e7cfu);
    int obj_y = 56;
    for (const auto& obj : simulation.objectives())
    {
        const std::string status = obj.completed ? "COMPLETED" : "PENDING";
        const std::uint32_t col = obj.completed ? 0xff9cba81u : 0xffe5e7cfu;
        text(18, obj_y, "OBJ " + std::to_string(obj.id) + " " + obj.text + " " + status, col);
        obj_y += 14;
    }
    if (!simulation.prompt_message().empty())
    {
        const auto& prompt = simulation.prompt_message();
        const int prompt_w = static_cast<int>(prompt.size() * 6 * 2) + 24;
        rectangle(center_x - prompt_w / 2, height_ - 68, prompt_w, 24, 0xff17211bu);
        text(center_x - prompt_w / 2 + 12, height_ - 62, prompt, 0xffffdd44u, 2);
    }
    else if (simulation.on_ladder())
    {
        text(center_x - 60, height_ - 75, "CLIMBING LADDER", 0xff66ccffu);
    }
    if (simulation.mission_completed())
    {
        rectangle(center_x - 180, center_y - 60, 360, 75, 0xff17211bu);
        text(center_x - 130, center_y - 45, "MISSION COMPLETE", 0xff9cba81u, 3);
        text(center_x - 120, center_y - 10, "OBJECTIVE COMPLETED", 0xffe5e7cfu, 2);
    }
    if (is_free_camera)
    {
        rectangle(center_x - 200, 14, 400, 42, 0xff17211bu);
        text(center_x - 185, 20, "FREE CAMERA ACTIVE F12 TO EXIT", 0xff66ccffu, 2);
        text(center_x - 185, 38, "WASD ROAM   SPACE UP   SHIFT DOWN", 0xffdce1d1u);
    }
    if (simulation.developer_mode())
    {
        const int dev_y = is_free_camera ? 60 : 14;
        rectangle(center_x - 210, dev_y, 420, 22, 0xff17211bu);
        text(center_x - 198, dev_y + 7, "DEV MODE: GOD MODE / ALL WEAPONS / INF AMMO", 0xff9cba81u);
    }
    if (simulation.damage_flash_remaining() > 0)
    {
        const int border = std::max(3, width_ / 200);
        rectangle(0, 0, width_, border, 0xffb44336u);
        rectangle(0, height_ - border, width_, border, 0xffb44336u);
        rectangle(0, 0, border, height_, 0xffb44336u);
        rectangle(width_ - border, 0, border, height_, 0xffb44336u);
        text(center_x - 48, center_y + 48, "TAKING FIRE", 0xffffb171u);
    }
    if (simulation.health() <= 0)
    {
        rectangle(center_x - 180, center_y - 50, 360, 90, 0xff17211bu);
        text(center_x - 120, center_y - 27, "MISSION FAILED", 0xffdab4a2u, 3);
        text(center_x - 90, center_y + 13, "ENTER TO RESTART", 0xffdce1d1u, 2);
    }
}

} // namespace igi::gameplay
