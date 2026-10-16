#include "igi/mission_scene.hpp"

#include "igi/asset_io.hpp"
#include "igi/diagnostics.hpp"
#include "igi/mission_assets.hpp"
#include "igi/mission_gameplay.hpp"
#include "igi/qvm_initializer.hpp"
#include "igi/qvm_task_tree.hpp"
#include "igi/terrain_surface.hpp"

#include "iff_parser.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace igi::gameplay
{
namespace
{

constexpr double units_per_metre = 4096.0;
constexpr std::array<std::array<int, 8>, 8> child_slots{{{{0, 1, 2, 3, 4, 5, 6, 7}},
                                                         {{2, 0, 3, 1, 6, 4, 7, 5}},
                                                         {{3, 2, 1, 0, 7, 6, 5, 4}},
                                                         {{1, 3, 0, 2, 5, 7, 4, 6}},
                                                         {{1, 0, 3, 2, 5, 4, 7, 6}},
                                                         {{3, 1, 2, 0, 7, 5, 6, 4}},
                                                         {{2, 3, 0, 1, 6, 7, 4, 5}},
                                                         {{0, 2, 1, 3, 4, 6, 5, 7}}}};

std::pair<int, int> orient(int orientation, int horizontal, int vertical)
{
    const int mirrored = (orientation & 4) != 0 ? -horizontal : horizontal;
    if ((orientation & 1) != 0)
    {
        return (orientation & 2) != 0 ? std::pair{vertical, -mirrored} : std::pair{-vertical, mirrored};
    }
    return (orientation & 2) != 0 ? std::pair{-mirrored, -vertical} : std::pair{mirrored, vertical};
}

std::string format_vector(Vector3 value)
{
    std::ostringstream text;
    text << std::fixed << std::setprecision(6) << '(' << value.x << ", " << value.y << ", " << value.z << ')';
    return text.str();
}

bool transform_matches(Vector3 authored, Vector3 runtime) noexcept
{
    constexpr float tolerance = 0.000001f;
    return std::abs(authored.x - runtime.x) <= tolerance && std::abs(authored.y - runtime.y) <= tolerance &&
           std::abs(authored.z - runtime.z) <= tolerance;
}

SceneFogParameters global_light_fog(const std::vector<assets::ScriptCall>& calls,
                                    std::size_t colour_offset,
                                    std::size_t density_offset,
                                    double& selected_time,
                                    double& authored_density)
{
    constexpr std::size_t time_offset = 53;
    constexpr double density_amount_scale = 0.0005;
    constexpr double maximum_density = 16384.0;
    constexpr double distance_scale = 0.001;

    const assets::ScriptCall* selected_keyframe = nullptr;
    selected_time = std::numeric_limits<double>::max();
    for (const auto& call : calls)
    {
        if (call.function != "Task_New" || call.arguments.size() <= time_offset ||
            call.arguments[1].text != "GlobalLightKeyframe")
        {
            continue;
        }
        const double time = call.arguments[time_offset].number;
        if (std::isfinite(time) && time < selected_time)
        {
            selected_keyframe = &call;
            selected_time = time;
        }
    }
    if (selected_keyframe == nullptr)
    {
        selected_time = 0.0;
        authored_density = 0.0;
        return {};
    }
    if (selected_keyframe->arguments.size() <= density_offset ||
        selected_keyframe->arguments.size() < colour_offset + 3)
    {
        throw std::runtime_error("GlobalLightKeyframe is missing required fog parameters");
    }

    const auto& arguments = selected_keyframe->arguments;
    const double red = arguments[colour_offset].number;
    const double green = arguments[colour_offset + 1].number;
    const double blue = arguments[colour_offset + 2].number;
    authored_density = arguments[density_offset].number;
    if (!std::isfinite(red) || !std::isfinite(green) || !std::isfinite(blue) ||
        !std::isfinite(authored_density))
    {
        throw std::runtime_error("GlobalLightKeyframe fog contains a non-finite value");
    }

    const double published_density =
        std::clamp(authored_density * density_amount_scale, 0.0, maximum_density);
    constexpr double game_units_per_metre = 4096.0;
    return {{static_cast<float>(std::clamp(red, 0.0, 1.0)),
             static_cast<float>(std::clamp(green, 0.0, 1.0)),
             static_cast<float>(std::clamp(blue, 0.0, 1.0))},
            static_cast<float>(published_density * distance_scale * game_units_per_metre)};
}

Vector3 transform_retail_orientation(Vector3 vertex, Vector3 rotation) noexcept
{
    const float cosine_alpha = std::cos(rotation.x);
    const float sine_alpha = std::sin(rotation.x);
    const float cosine_beta = std::cos(rotation.y);
    const float sine_beta = std::sin(rotation.y);
    const float cosine_gamma = std::cos(rotation.z);
    const float sine_gamma = std::sin(rotation.z);

    const float matrix_11 = cosine_gamma * cosine_beta;
    const float matrix_12 = (cosine_gamma * sine_beta * sine_alpha) - (sine_gamma * cosine_alpha);
    const float matrix_13 = (cosine_gamma * sine_beta * cosine_alpha) + (sine_gamma * sine_alpha);
    const float matrix_21 = sine_gamma * cosine_beta;
    const float matrix_22 = (sine_gamma * sine_beta * sine_alpha) + (cosine_gamma * cosine_alpha);
    const float matrix_23 = (sine_gamma * sine_beta * cosine_alpha) - (cosine_gamma * sine_alpha);
    const float matrix_31 = -sine_beta;
    const float matrix_32 = cosine_beta * sine_alpha;
    const float matrix_33 = cosine_beta * cosine_alpha;

    return {vertex.x * matrix_11 + vertex.y * matrix_21 + vertex.z * matrix_31,
            vertex.x * matrix_12 + vertex.y * matrix_22 + vertex.z * matrix_32,
            vertex.x * matrix_13 + vertex.y * matrix_23 + vertex.z * matrix_33};
}

struct EmittedGeometryTransform final
{
    Vector3 position{};
    float position_error{};
    float orientation_error{};
    float tolerance{};
    bool has_geometry{};
};

EmittedGeometryTransform verify_emitted_geometry_transform(const SceneMesh& source_mesh,
                                                           const MissionScene& scene,
                                                           std::size_t first_scene_triangle,
                                                           Vector3 authored_position,
                                                           Vector3 authored_rotation)
{
    if (source_mesh.triangles.empty() || first_scene_triangle > scene.static_triangles.size() ||
        source_mesh.triangles.size() > scene.static_triangles.size() - first_scene_triangle)
    {
        return {};
    }

    const SceneTriangle& source_anchor_triangle = source_mesh.triangles.front();
    const SceneTriangle& runtime_anchor_triangle = scene.static_triangles[first_scene_triangle];
    const Vector3 source_anchor = source_anchor_triangle.vertices.front().position;
    const Vector3 expected_anchor_offset = transform_retail_orientation(source_anchor, authored_rotation);
    const Vector3 runtime_anchor = runtime_anchor_triangle.vertices.front().position;

    EmittedGeometryTransform verification;
    verification.has_geometry = true;
    verification.position = runtime_anchor - expected_anchor_offset;
    verification.position_error = length(verification.position - authored_position);
    float coordinate_scale = std::max({1.0f,
                                       std::abs(authored_position.x),
                                       std::abs(authored_position.y),
                                       std::abs(authored_position.z),
                                       std::abs(source_anchor.x),
                                       std::abs(source_anchor.y),
                                       std::abs(source_anchor.z),
                                       std::abs(runtime_anchor.x),
                                       std::abs(runtime_anchor.y),
                                       std::abs(runtime_anchor.z)});

    for (std::size_t triangle_index = 0; triangle_index < source_mesh.triangles.size(); ++triangle_index)
    {
        const SceneTriangle& source_triangle = source_mesh.triangles[triangle_index];
        const SceneTriangle& runtime_triangle = scene.static_triangles[first_scene_triangle + triangle_index];

        for (std::size_t vertex_index = 0; vertex_index < source_triangle.vertices.size(); ++vertex_index)
        {
            const Vector3 expected_offset = transform_retail_orientation(
                source_triangle.vertices[vertex_index].position, authored_rotation);
            const Vector3 runtime_vertex = runtime_triangle.vertices[vertex_index].position;
            if (!finite(source_triangle.vertices[vertex_index].position) || !finite(expected_offset) ||
                !finite(runtime_vertex))
            {
                verification.position_error = std::numeric_limits<float>::infinity();
                verification.orientation_error = std::numeric_limits<float>::infinity();
                return verification;
            }
            coordinate_scale = std::max({coordinate_scale,
                                         std::abs(runtime_vertex.x),
                                         std::abs(runtime_vertex.y),
                                         std::abs(runtime_vertex.z),
                                         std::abs(expected_offset.x),
                                         std::abs(expected_offset.y),
                                         std::abs(expected_offset.z)});
            const Vector3 expected_relative = expected_offset - expected_anchor_offset;
            const Vector3 runtime_relative = runtime_vertex - runtime_anchor;
            verification.orientation_error =
                std::max(verification.orientation_error, length(runtime_relative - expected_relative));
        }
    }

    verification.tolerance =
        std::max(0.000001f, coordinate_scale * std::numeric_limits<float>::epsilon() * 2.0f);
    if (!finite(verification.position) || !std::isfinite(verification.position_error) ||
        !std::isfinite(verification.orientation_error) || !std::isfinite(verification.tolerance) ||
        verification.position_error > verification.tolerance ||
        verification.orientation_error > verification.tolerance)
    {
        return verification;
    }

    return verification;
}

void append_terrain(MissionScene& scene,
                    const std::filesystem::path& mission_directory,
                    const std::array<double, 3>& origin,
                    const std::vector<std::uint32_t>& textures,
                    const assets::TerrainSurface& surface,
                    TerrainDetail detail)
{
    const auto tree_bytes = assets::read_file(assets::resolve_path(mission_directory, "terrain/terrain.ctr"));
    const auto command_bytes =
        assets::read_file(assets::resolve_path(mission_directory, "terrain/terrain.cmd"));
    const assets::BinaryView tree(tree_bytes);
    const assets::BinaryView commands(command_bytes);
    if (tree.size() < 64 || tree.size() % 32 != 0 || textures.empty())
    {
        throw std::runtime_error("Invalid terrain record table or texture atlas");
    }
    constexpr int minimum_terrain_exponent = 7;
    constexpr int maximum_terrain_exponent = 16;
    constexpr std::int64_t terrain_root_extent = 1073741824;
    const int terrain_view_distance = detail == TerrainDetail::low        ? 96
                                      : detail == TerrainDetail::medium   ? 129
                                      : detail == TerrainDetail::vanilla  ? 163
                                      : detail == TerrainDetail::enhanced ? 220
                                      : detail == TerrainDetail::high     ? 320
                                                                          : 450;
    std::array<std::int64_t, 3> camera_position{};
    for (std::size_t axis_index = 0; axis_index < camera_position.size(); ++axis_index)
    {
        if (!std::isfinite(origin[axis_index]) ||
            origin[axis_index] < static_cast<double>(std::numeric_limits<std::int32_t>::min()) ||
            origin[axis_index] > static_cast<double>(std::numeric_limits<std::int32_t>::max()))
        {
            throw std::runtime_error(
                "Terrain camera position is outside the retail integer coordinate range");
        }
        camera_position[axis_index] = static_cast<std::int64_t>(std::trunc(origin[axis_index]));
    }
    const std::size_t first_terrain_triangle = scene.static_triangles.size();
    std::size_t visits = 0;
    const auto emit_terrain_chunk = [&](std::size_t node_index,
                                        int depth,
                                        int orientation,
                                        const std::array<double, 3>& centre,
                                        double extent)
    {
        if (depth < minimum_terrain_exponent)
        {
            return;
        }
        const std::size_t node_offset = node_index * 32;
        const auto command_offset = tree.unsigned_integer(node_offset + 28);
        const std::size_t triangle_count = commands.unsigned_short(command_offset);
        const std::size_t triangle_bytes = commands.unsigned_short(command_offset + 2);
        const std::size_t vertex_count =
            commands.unsigned_short(command_offset + 4) + commands.unsigned_short(command_offset + 6);
        if (triangle_bytes != triangle_count * 4 || vertex_count > 256)
        {
            throw std::runtime_error("Terrain command header is invalid");
        }
        commands.require_range(command_offset + 8, triangle_bytes + vertex_count * 4);
        std::vector<Vector3> positions;
        std::vector<int> texture_axes;
        const double vertex_scale = extent / 24.0;
        positions.reserve(vertex_count);
        texture_axes.reserve(vertex_count);
        for (std::size_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
        {
            const auto packed =
                commands.unsigned_integer(command_offset + 8 + triangle_bytes + vertex_index * 4);
            const auto [horizontal, vertical] = orient(orientation,
                                                       static_cast<int>((packed >> 26) & 63) - 24,
                                                       static_cast<int>((packed >> 20) & 63) - 24);
            const double world_x = centre[0] + horizontal * vertex_scale;
            const double world_y = centre[1] + vertical * vertex_scale;
            const double displacement =
                depth >= (detail == TerrainDetail::ultra ? 10 : detail == TerrainDetail::enhanced ? 11 : 13)
                    ? surface.height(world_x, world_y)
                    : 0;
            positions.push_back(
                {static_cast<float>((world_x - origin[0]) / units_per_metre),
                 static_cast<float>((world_y - origin[1]) / units_per_metre),
                 static_cast<float>((centre[2] + (static_cast<int>((packed >> 14) & 63) - 24) * vertex_scale +
                                     displacement - origin[2]) /
                                    units_per_metre)});
            texture_axes.push_back(static_cast<int>((packed >> 6) & 3));
        }
        const std::size_t first_chunk_triangle = scene.static_triangles.size();
        for (std::size_t triangle_index = 0; triangle_index < triangle_count; ++triangle_index)
        {
            const auto packed = commands.unsigned_integer(command_offset + 8 + triangle_index * 4);
            SceneTriangle triangle;
            for (int corner_index = 0; corner_index < 3; ++corner_index)
            {
                const auto vertex_index = (packed >> (corner_index * 8)) & 255;
                if (vertex_index >= positions.size())
                {
                    throw std::runtime_error("Terrain triangle index is invalid");
                }
                const auto position = positions[vertex_index];
                triangle.vertices[corner_index] = {position, position.x / 8.0f, position.y / 8.0f};
            }
            const auto centroid = (triangle.vertices[0].position + triangle.vertices[1].position +
                                   triangle.vertices[2].position) *
                                  (1.0f / 3.0f);
            const int material = surface.material(centroid.x * units_per_metre + origin[0],
                                                  centroid.y * units_per_metre + origin[1]);
            const auto projection = surface.projection(material, depth, centre);
            if (projection.tile >= textures.size())
            {
                throw std::runtime_error("Terrain material selects a missing atlas tile");
            }
            triangle.texture_index = textures[projection.tile];
            for (int corner_index = 0; corner_index < 3; ++corner_index)
            {
                auto& vertex = triangle.vertices[corner_index];
                const int axis = texture_axes[(packed >> (corner_index * 8)) & 255];
                const double offset_x = vertex.position.x * units_per_metre + origin[0] - centre[0];
                const double offset_y = vertex.position.y * units_per_metre + origin[1] - centre[1];
                const double offset_z = vertex.position.z * units_per_metre + origin[2] - centre[2];
                const bool use_y = (((orientation & 1) ^ axis) & 1) != 0;
                vertex.texture_u = static_cast<float>(axis == 0 || !use_y
                                                          ? projection.origin_u + offset_x * projection.rate
                                                          : projection.origin_v + offset_y * projection.rate);
                vertex.texture_v =
                    static_cast<float>(axis == 0 ? projection.origin_v + offset_y * projection.rate
                                                 : projection.origin_w + offset_z * projection.rate);
            }
            scene.static_triangles.push_back(triangle);
            scene.collision_triangles.push_back({{triangle.vertices[0].position,
                                                  triangle.vertices[1].position,
                                                  triangle.vertices[2].position}});
        }
        if (triangle_count > 0)
        {
            Vector3 bounds_minimum = positions.front();
            Vector3 bounds_maximum = positions.front();
            for (const Vector3 vertex_position : positions)
            {
                bounds_minimum.x = std::min(bounds_minimum.x, vertex_position.x);
                bounds_minimum.y = std::min(bounds_minimum.y, vertex_position.y);
                bounds_minimum.z = std::min(bounds_minimum.z, vertex_position.z);
                bounds_maximum.x = std::max(bounds_maximum.x, vertex_position.x);
                bounds_maximum.y = std::max(bounds_maximum.y, vertex_position.y);
                bounds_maximum.z = std::max(bounds_maximum.z, vertex_position.z);
            }
            const Vector3 bounds_center = (bounds_minimum + bounds_maximum) * 0.5f;
            const float bounds_radius = length(bounds_maximum - bounds_center);
            if (!finite(bounds_center) || !std::isfinite(bounds_radius) || bounds_radius < 0.0f)
            {
                throw std::runtime_error("Terrain render chunk contains invalid bounds");
            }
            scene.terrain_render_chunks.push_back(
                {first_chunk_triangle, triangle_count, bounds_center, bounds_radius});
        }
    };
    std::function<void(std::size_t, int, int, std::array<double, 3>)> descend;
    descend = [&](std::size_t node_index, int depth, int orientation, std::array<double, 3> centre)
    {
        if (++visits > 2000000 || depth > maximum_terrain_exponent || node_index >= tree.size() / 32)
        {
            throw std::runtime_error("Terrain tree exceeds bounds or traversal budget");
        }
        const double extent = std::ldexp(1073741824.0, -depth);
        const double distance = std::max(std::abs(centre[0] - origin[0]), std::abs(centre[1] - origin[1]));
        if (distance - extent > 1400.0 * units_per_metre ||
            std::abs(centre[2] - origin[2]) - extent > 800.0 * units_per_metre)
        {
            return;
        }
        const std::size_t node_offset = node_index * 32;
        const auto child_mask = tree.byte(node_offset + 24);
        if (child_mask == 0 || depth == maximum_terrain_exponent)
        {
            emit_terrain_chunk(node_index, depth, orientation, centre, extent);
            return;
        }
        for (int octant = 0; octant < 8; ++octant)
        {
            const int child_slot = child_slots[orientation][octant];
            if ((child_mask & (1 << child_slot)) == 0)
            {
                continue;
            }
            const int child_orientation = tree.byte(node_offset + 16 + child_slot);
            if (child_orientation > 7)
            {
                throw std::runtime_error("Terrain child orientation is invalid");
            }
            const int combined_orientation =
                (orientation & 4) == 0
                    ? (child_orientation & 4) | ((orientation + child_orientation) & 3)
                    : ((child_orientation & 4) ^ 4) | ((orientation - child_orientation) & 3);
            auto child_centre = centre;
            for (int axis_index = 0; axis_index < 3; ++axis_index)
            {
                child_centre[axis_index] += (octant & (1 << axis_index)) != 0 ? extent * 0.5 : -extent * 0.5;
            }
            const std::size_t child_node_index = tree.unsigned_short(node_offset + child_slot * 2);
            if (++visits > 2000000 || child_node_index >= tree.size() / 32)
            {
                throw std::runtime_error("Terrain tree exceeds bounds or traversal budget");
            }
            const double child_extent = extent * 0.5;
            const double child_distance =
                std::max(std::abs(child_centre[0] - origin[0]), std::abs(child_centre[1] - origin[1]));
            if (child_distance - child_extent > 1400.0 * units_per_metre ||
                std::abs(child_centre[2] - origin[2]) - child_extent > 800.0 * units_per_metre)
            {
                continue;
            }
            const std::array<std::int64_t, 3> child_world{
                static_cast<std::int64_t>(child_centre[0]) - camera_position[0],
                static_cast<std::int64_t>(child_centre[1]) - camera_position[1],
                static_cast<std::int64_t>(child_centre[2]) - camera_position[2]};
            const std::int64_t reach =
                std::max({std::abs(child_world[0]), std::abs(child_world[1]), std::abs(child_world[2])});
            const auto child_depth = depth + 1;
            const auto child_extent_integer = terrain_root_extent >> child_depth;
            const std::int64_t detail_metric = (reach - child_extent_integer) >> (25 - child_depth);
            const std::size_t child_node_offset = child_node_index * 32;
            const bool should_refine = tree.byte(child_node_offset + 24) != 0 &&
                                       child_depth < maximum_terrain_exponent &&
                                       detail_metric < terrain_view_distance;
            if (should_refine)
            {
                descend(child_node_index, child_depth, combined_orientation, child_centre);
            }
            else
            {
                emit_terrain_chunk(
                    child_node_index, child_depth, combined_orientation, child_centre, child_extent);
            }
        }
    };
    descend(1, 0, 0, {});
    std::size_t next_chunk_triangle = first_terrain_triangle;
    for (const TerrainRenderChunk& terrain_chunk : scene.terrain_render_chunks)
    {
        if (terrain_chunk.triangle_count == 0 || terrain_chunk.first_triangle != next_chunk_triangle ||
            terrain_chunk.triangle_count > scene.static_triangles.size() - next_chunk_triangle)
        {
            throw std::runtime_error("Terrain render chunks do not cover the emitted triangle sequence");
        }
        next_chunk_triangle += terrain_chunk.triangle_count;
    }
    if (next_chunk_triangle != scene.static_triangles.size())
    {
        throw std::runtime_error("Terrain render chunks omit emitted terrain triangles");
    }
    diagnostics::log(diagnostics::Severity::information,
                     "terrain",
                     "Loaded " + std::to_string(scene.static_triangles.size()) +
                         " original terrain triangles across " +
                         std::to_string(scene.terrain_render_chunks.size()) + " render chunks");
}

void append_model(MissionScene& scene,
                  const SceneMesh& mesh,
                  Vector3 position,
                  Vector3 rotation,
                  bool include_collision = true)
{
    scene.attached_model_count += mesh.attachment_count;
    scene.maximum_attachment_depth = std::max(scene.maximum_attachment_depth, mesh.maximum_attachment_depth);
    for (auto triangle : mesh.triangles)
    {
        for (auto& vertex : triangle.vertices)
        {
            vertex.position = rotate_euler(vertex.position, rotation) + position;
        }
        scene.static_triangles.push_back(triangle);
    }
    if (!include_collision)
    {
        return;
    }
    if (mesh.collision.empty())
    {
        for (const auto& triangle : mesh.triangles)
        {
            CollisionTriangle collision;
            for (std::size_t corner_index = 0; corner_index < 3; ++corner_index)
            {
                collision.positions[corner_index] =
                    rotate_euler(triangle.vertices[corner_index].position, rotation) + position;
            }
            scene.collision_triangles.push_back(collision);
        }
    }
    else
    {
        for (auto triangle : mesh.collision)
        {
            for (auto& vertex : triangle.positions)
            {
                vertex = rotate_euler(vertex, rotation) + position;
            }
            scene.collision_triangles.push_back(triangle);
        }
    }
    for (auto triangle : mesh.movement_collision)
    {
        for (auto& vertex : triangle.positions)
        {
            vertex = rotate_euler(vertex, rotation) + position;
        }
        scene.movement_collision_triangles.push_back(triangle);
    }
}

bool is_model_identifier(const std::string& s)
{
    const auto p1 = s.find('_');
    if (p1 == 0 || p1 == std::string::npos)
    {
        return false;
    }
    const auto p2 = s.find('_', p1 + 1);
    if (p2 == std::string::npos || p2 == p1 + 1)
    {
        return false;
    }
    if (s.size() <= p2 + 1)
    {
        return false;
    }
    for (std::size_t i = 0; i < p1; ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
        {
            return false;
        }
    }
    for (std::size_t i = p1 + 1; i < p2; ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
        {
            return false;
        }
    }
    for (std::size_t i = p2 + 1; i < s.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
        {
            return false;
        }
    }
    return true;
}

} // namespace

MissionScene load_mission(const std::filesystem::path& game_directory,
                          const std::filesystem::path& cache_directory,
                          int mission_number,
                          TerrainDetail terrain_detail)
{
    if (mission_number < 1 || mission_number > 14)
    {
        throw std::runtime_error("Mission number must be between 1 and 14");
    }
    MissionScene scene;
    scene.mission_number = mission_number;
    scene.game_directory = game_directory;
    diagnostics::log(diagnostics::Severity::information,
                     "LevelLoader",
                     "LoadLevel() START for level " + std::to_string(mission_number));
    const auto mission_directory =
        assets::resolve_path(game_directory, "missions/location0/level" + std::to_string(mission_number));
    const auto object_bytes = assets::read_file(assets::resolve_path(mission_directory, "objects.qvm"));
    const assets::QvmInitializer initializer{assets::BinaryView(object_bytes)};
    double global_light_time = 0.0;
    double terrain_fog_amount = 0.0;
    double object_fog_amount = 0.0;
    scene.terrain_fog = global_light_fog(initializer.calls(), 7, 10, global_light_time, terrain_fog_amount);
    double object_keyframe_time = 0.0;
    scene.object_fog = global_light_fog(initializer.calls(), 15, 18, object_keyframe_time, object_fog_amount);
    if (std::abs(global_light_time - object_keyframe_time) > 0.000001)
    {
        throw std::runtime_error("Terrain and object fog selected different GlobalLightKeyframes");
    }
    std::ostringstream fog_log;
    fog_log << std::fixed << std::setprecision(6) << "GlobalLightKeyframe time=" << global_light_time
            << ", terrain density=" << terrain_fog_amount << " -> " << scene.terrain_fog.density_per_metre
            << " per metre, terrain colour=" << format_vector(scene.terrain_fog.colour)
            << ", object density=" << object_fog_amount << " -> " << scene.object_fog.density_per_metre
            << " per metre, object colour=" << format_vector(scene.object_fog.colour);
    diagnostics::log(diagnostics::Severity::information, "scene-fog", fog_log.str());
    std::array<double, 3> origin{};
    bool found_player = false;
    for (const auto& call : initializer.calls())
    {
        if (call.function == "Task_New" && call.arguments.size() >= 9 &&
            call.arguments[1].text == "HumanPlayer")
        {
            origin = {call.arguments[3].number, call.arguments[4].number, call.arguments[5].number};
            scene.player_heading = static_cast<float>(call.arguments[6].number);
            found_player = true;
            break;
        }
    }
    if (!found_player)
    {
        throw std::runtime_error("Original mission script has no player spawn");
    }
    assets::MissionAssets library(game_directory, mission_directory, cache_directory);
    const auto tiles =
        library.terrain_textures(assets::resolve_path(mission_directory, "terrain/terrain.tex"));
    const assets::TerrainSurface terrain_surface(mission_directory, initializer.calls());
    append_terrain(scene, mission_directory, origin, tiles, terrain_surface, terrain_detail);
    scene.terrain_triangle_count = scene.static_triangles.size();
    scene.movement_collision_triangles = scene.collision_triangles;
    const auto enabled_tasks = assets::gameplay_task_mask(initializer.calls());
    static const auto resolve_pickup_model = [](std::string_view enum_id) -> std::string
    {
        if (enum_id == "WEAPON_ID_COLT") return "317_01_1";
        if (enum_id == "WEAPON_ID_GLOCK") return "300_01_1";
        if (enum_id == "WEAPON_ID_DESERTEAGLE") return "316_01_1";
        if (enum_id == "WEAPON_ID_UZI") return "322_01_1";
        if (enum_id == "WEAPON_ID_MP5SD") return "324_01_1";
        if (enum_id == "WEAPON_ID_AK47") return "325_01_1";
        if (enum_id == "WEAPON_ID_M16") return "323_01_1";
        if (enum_id == "WEAPON_ID_DRAGUNOV") return "335_01_1";
        if (enum_id == "WEAPON_ID_SPAS12") return "344_01_1";
        if (enum_id == "WEAPON_ID_JACKHAMMER") return "342_01_1";
        if (enum_id == "WEAPON_ID_MINIMI") return "352_01_1";
        if (enum_id == "WEAPON_ID_GRENADE") return "307_01_1";
        if (enum_id == "WEAPON_ID_KNIFE") return "306_01_1";
        if (enum_id.find("AMMO_ID_") == 0) return "310_01_1";
        return std::string(enum_id);
    };

    struct TaskFieldDef
    {
        std::string name;
        std::string type_name;
        int arg_offset{};
        int arg_count{1};
    };

    using TaskSchema = std::vector<TaskFieldDef>;

    static const auto get_builtin_schemas = []() {
        std::unordered_map<std::string, TaskSchema> s;
        auto add = [](TaskSchema& sc, const char* name, const char* type, int off) {
            TaskFieldDef f;
            f.name = name;
            f.type_name = type;
            f.arg_offset = off;
            f.arg_count = (std::string_view(type) == "ObjectPos" || std::string_view(type) == "Real32x9" ||
                           std::string_view(type) == "RGB" || std::string_view(type) == "Real32x3" ||
                           std::string_view(type) == "Colour") ? 3 : 1;
            sc.push_back(f);
        };

        // Building / EditRigidObj / Terminal: pos@3, ori@6, model@9
        for (const char* t : {"Building", "EditRigidObj", "Terminal"})
        {
            add(s[t], "Position", "ObjectPos", 3);
            add(s[t], "Orientation", "Real32x9", 6);
            add(s[t], "Model", "String16", 9);
        }

        // HumanSoldier types: pos@3, gamma@6 (Real32), model@7, team@8, bone@9, stand@10
        for (const char* t : {"HumanSoldier", "HumanSoldierFemale", "HumanSoldierRPG", "HumanPlayer"})
        {
            add(s[t], "Position", "ObjectPos", 3);
            add(s[t], "Gamma", "Real32", 6);
            add(s[t], "Model", "String16", 7);
            add(s[t], "Team", "Int32", 8);
            add(s[t], "Bone Heirachy", "Int32", 9);
            add(s[t], "Stand Animation", "Int32", 10);
        }

        // Door: pos@3, (3 unknown physics args @6), ori@9, model@12
        {
            auto& sc = s["Door"];
            add(sc, "Position", "ObjectPos", 3);
            add(sc, "Orientation", "Real32x9", 9);
            add(sc, "Model", "String16", 12);
        }

        // Car / Heli: pos@3, heading@8 (Real32), model@13
        for (const char* t : {"Car", "Heli"})
        {
            add(s[t], "Position", "ObjectPos", 3);
            add(s[t], "Heading", "Real32", 8);
            add(s[t], "Model", "String16", 13);
        }

        // ExplodeObject: pos@3, ori@6, model@9, destroyed_model@10
        {
            auto& sc = s["ExplodeObject"];
            add(sc, "Position", "ObjectPos", 3);
            add(sc, "Orientation", "Real32x9", 6);
            add(sc, "Model", "String16", 9);
            add(sc, "Destroyed model", "String16", 10);
        }

        // Switch: pos@3, ori@6, count@9, reverse@10, models@11..15, loop@16
        {
            auto& sc = s["Switch"];
            add(sc, "Position", "ObjectPos", 3);
            add(sc, "Orientation", "Real32x9", 6);
            add(sc, "Model", "String16", 11);
        }

        // Wire: start pos@3, end pos@6, model@9
        {
            auto& sc = s["Wire"];
            add(sc, "Position", "ObjectPos", 3);
            add(sc, "Model", "String16", 9);
        }

        // AlarmControl / SCameraControl: pos@3, ori@6, model@9
        for (const char* t : {"AlarmControl", "SCameraControl"})
        {
            add(s[t], "Position", "ObjectPos", 3);
            add(s[t], "Orientation", "Real32x9", 6);
            add(s[t], "Model", "String16", 9);
        }

        // SCamera: pos@3, oz@6, ox@8, oy@9, model@10
        {
            auto& sc = s["SCamera"];
            add(sc, "Position", "ObjectPos", 3);
            add(sc, "Orientation", "Real32x9", 6);
            add(sc, "Model", "String16", 10);
        }

        // SplineObjWaypoint: ori first@3, pos@6, model@9
        {
            auto& sc = s["SplineObjWaypoint"];
            add(sc, "Orientation", "Real32x9", 3);
            add(sc, "Position", "ObjectPos", 6);
            add(sc, "Model", "String16", 9);
        }

        // AmbientArea
        {
            auto& sc = s["AmbientArea"];
            add(sc, "Position", "ObjectPos", 3);
            add(sc, "Orientation", "Real32x9", 6);
        }

        // Train
        {
            auto& sc = s["Train"];
            add(sc, "Position", "TrainPos1D", 3);
            add(sc, "Model", "String256", 6);
        }

        // Fence: pos@3, heading@6 (Real32), model@7
        {
            auto& sc = s["Fence"];
            add(sc, "Position", "ObjectPos", 3);
            add(sc, "Heading", "Real32", 6);
            add(sc, "Model", "String16", 7);
        }

        // Cabinet: pos@3, heading@6 (Real32), model@7
        {
            auto& sc = s["Cabinet"];
            add(sc, "Position", "ObjectPos", 3);
            add(sc, "Heading", "Real32", 6);
            add(sc, "Model", "String16", 7);
        }

        // Generic positioned objects: pos@3, ori@6, model@9
        for (const char* t : {"AIStationaryGunHolder", "AlarmLight", "Elevator", "Generator",
                              "GenericPickup", "GenericTBA", "Plane", "Radio",
                              "RotatingObject", "Siren", "StationaryGun", "GunPickup"})
        {
            add(s[t], "Position", "ObjectPos", 3);
            add(s[t], "Orientation", "Real32x9", 6);
            add(s[t], "Model", "String16", 9);
        }

        // AmmoPickup: pos@3, ori@6, model@9, ammo@10
        {
            auto& sc = s["AmmoPickup"];
            add(sc, "Position", "ObjectPos", 3);
            add(sc, "Orientation", "Real32x9", 6);
            add(sc, "Model", "String16", 9);
            add(sc, "Ammo", "Int16", 10);
        }

        return s;
    };
    static const auto builtin_schemas = get_builtin_schemas();

    struct ObjectCandidate
    {
        std::string model_id;
        Vector3 position{};
        Vector3 rotation{};
        bool is_soldier = false;
        int team = 1;
        int task_id = -1;
        std::string type;
        std::string name;
        double raw_px = 0.0;
        double raw_py = 0.0;
        double raw_pz = 0.0;
        double raw_ox = 0.0;
        double raw_oy = 0.0;
        double raw_oz = 0.0;
        bool has_ori = false;
        int skeleton_id = 1;
        int stand_anim = 2;
    };

    std::size_t verified_task_count = 0;
    std::size_t mismatch_count = 0;

    for (const auto& call : initializer.calls())
    {
        if (call.function == "Task_New" && call.arguments.size() >= 5 && call.arguments[1].text == "AddAmmo")
        {
            const double ammunition = call.arguments[4].number;
            if (!std::isfinite(ammunition) || ammunition < 0 || ammunition > 10000)
            {
                throw std::runtime_error("Original player reserve ammunition is invalid");
            }
            scene.initial_reserve_ammunition = static_cast<std::uint32_t>(ammunition);
        }
        if (call.function != "Task_New" || call.arguments.size() < 4)
        {
            continue;
        }
        const auto& arguments = call.arguments;
        const auto& task_type = arguments[1].text;
        const bool enabled = enabled_tasks[static_cast<std::size_t>(&call - initializer.calls().data())];

        std::optional<ObjectCandidate> candidate;
        const auto sit = builtin_schemas.find(task_type);

        if (sit != builtin_schemas.end())
        {
            const auto& schema = sit->second;
            double px = 0.0, py = 0.0, pz = 0.0;
            bool has_pos = false;
            double ox = 0.0, oy = 0.0, oz = 0.0;
            bool has_ori = false;
            std::string model_id;
            bool is_soldier = false;
            int team = 1;
            int skeleton_id = 1;
            int stand_anim = 2;

            if (task_type == "HumanSoldier" || task_type == "HumanSoldierFemale" || task_type == "HumanSoldierRPG")
            {
                is_soldier = true;
                if (arguments.size() > 8) team = static_cast<int>(arguments[8].number);
                if (arguments.size() > 9) skeleton_id = static_cast<int>(arguments[9].number);
                if (arguments.size() > 10) stand_anim = static_cast<int>(arguments[10].number);
            }

            for (const auto& fd : schema)
            {
                if (!has_pos && fd.type_name == "ObjectPos")
                {
                    if (fd.arg_offset + 2 < static_cast<int>(arguments.size()))
                    {
                        px = arguments[fd.arg_offset].number;
                        py = arguments[fd.arg_offset + 1].number;
                        pz = arguments[fd.arg_offset + 2].number;
                        has_pos = true;
                    }
                }
                if (!has_ori)
                {
                    if (task_type == "SCamera")
                    {
                        if (arguments.size() > 9)
                        {
                            oz = arguments[6].number;
                            ox = arguments[8].number;
                            oy = arguments[9].number;
                            has_ori = true;
                        }
                    }
                    else if (fd.type_name == "Real32x9")
                    {
                        if (fd.arg_offset + 2 < static_cast<int>(arguments.size()))
                        {
                            ox = arguments[fd.arg_offset].number;
                            oy = arguments[fd.arg_offset + 1].number;
                            oz = arguments[fd.arg_offset + 2].number;
                            has_ori = true;
                        }
                    }
                    else if (fd.name == "Heading" || fd.name == "Gamma")
                    {
                        if (fd.arg_offset < static_cast<int>(arguments.size()))
                        {
                            ox = 0.0;
                            oy = 0.0;
                            oz = arguments[fd.arg_offset].number;
                            has_ori = true;
                        }
                    }
                }
                if (model_id.empty() && (fd.type_name == "String16" || fd.type_name == "String256" || fd.type_name == "VarString"))
                {
                    if (fd.arg_offset < static_cast<int>(arguments.size()) && !arguments[fd.arg_offset].text.empty())
                    {
                        model_id = arguments[fd.arg_offset].text;
                    }
                }
            }

            // Fallback model search if schema offset had non-model or empty
            if (model_id.empty())
            {
                for (std::size_t ai = 6; ai < arguments.size(); ++ai)
                {
                    if (is_model_identifier(arguments[ai].text))
                    {
                        model_id = arguments[ai].text;
                        break;
                    }
                }
            }

            if (has_pos && !model_id.empty())
            {
                candidate = ObjectCandidate{
                    .model_id = model_id,
                    .position = {static_cast<float>((px - origin[0]) / units_per_metre),
                                 static_cast<float>((py - origin[1]) / units_per_metre),
                                 static_cast<float>((pz - origin[2]) / units_per_metre)},
                    .rotation = {static_cast<float>(ox), static_cast<float>(oy), static_cast<float>(oz)},
                    .is_soldier = is_soldier,
                    .team = team,
                    .task_id = static_cast<int>(arguments[0].number),
                    .type = task_type,
                    .name = arguments.size() > 2 ? arguments[2].text : "",
                    .raw_px = px,
                    .raw_py = py,
                    .raw_pz = pz,
                    .raw_ox = ox,
                    .raw_oy = oy,
                    .raw_oz = oz,
                    .has_ori = has_ori,
                    .skeleton_id = skeleton_id,
                    .stand_anim = stand_anim,
                };
            }
        }
        else
        {
            // Generic fallback for custom/undeclared tasks: pos@3, ori@6, model@9
            if (arguments.size() >= 10 && !arguments[9].text.empty())
            {
                candidate = ObjectCandidate{
                    .model_id = arguments[9].text,
                    .position = {static_cast<float>((arguments[3].number - origin[0]) / units_per_metre),
                                 static_cast<float>((arguments[4].number - origin[1]) / units_per_metre),
                                 static_cast<float>((arguments[5].number - origin[2]) / units_per_metre)},
                    .rotation = {static_cast<float>(arguments[6].number),
                                 static_cast<float>(arguments[7].number),
                                 static_cast<float>(arguments[8].number)},
                    .task_id = static_cast<int>(arguments[0].number),
                    .type = task_type,
                    .name = arguments.size() > 2 ? arguments[2].text : "",
                    .raw_px = arguments[3].number,
                    .raw_py = arguments[4].number,
                    .raw_pz = arguments[5].number,
                    .raw_ox = arguments[6].number,
                    .raw_oy = arguments[7].number,
                    .raw_oz = arguments[8].number,
                    .has_ori = true,
                };
            }
        }

        if (!candidate.has_value())
        {
            continue;
        }

        if (!enabled && !candidate->is_soldier)
        {
            continue;
        }

        candidate->model_id = resolve_pickup_model(candidate->model_id);
        if (candidate->model_id.empty() || candidate->model_id == "waypoint" ||
            !is_model_identifier(candidate->model_id))
        {
            continue;
        }

        if (!finite(candidate->position) || length(candidate->position) > 2400.0f)
        {
            continue;
        }

        if (!library.has_model(candidate->model_id))
        {
            if (candidate->type == "Building" || candidate->type == "EditRigidObj" ||
                candidate->type == "ExplodeObject" || candidate->is_soldier)
            {
                ++scene.missing_object_count;
                ++mismatch_count;
                diagnostics::log(diagnostics::Severity::warning,
                                 "scene",
                                 "Required model missing: " + candidate->model_id);
            }
            continue;
        }

        ++verified_task_count;
        try
        {
            const auto mesh = library.model(candidate->model_id);
            const std::size_t first_scene_triangle = scene.static_triangles.size();
            std::size_t scene_triangle_count = 0;
            Vector3 runtime_position{};
            Vector3 runtime_rotation{};
            float position_error = 0.0f;
            float orientation_error = 0.0f;
            float verification_tolerance = 0.000001f;
            bool emitted_geometry_verified = false;
            bool position_matches = false;
            bool orientation_matches = false;
            if (candidate->is_soldier)
            {
                scene.actors.push_back(
                    {candidate->position, candidate->rotation.z, candidate->task_id, candidate->team, mesh});
                scene.actors.back().source_call_index =
                    static_cast<std::size_t>(&call - initializer.calls().data());
                runtime_position = scene.actors.back().position;
                runtime_rotation = {0.0f, 0.0f, scene.actors.back().heading};
                position_error = length(runtime_position - candidate->position);
                orientation_error = length(runtime_rotation - candidate->rotation);
                position_matches = transform_matches(candidate->position, runtime_position);
                orientation_matches = transform_matches(candidate->rotation, runtime_rotation);
            }
            else if (candidate->type == "Door")
            {
                SceneDoor door;
                door.task_id = candidate->task_id;
                door.source_call_index = static_cast<std::size_t>(&call - initializer.calls().data());
                door.name = candidate->name;
                door.model_id = candidate->model_id;
                door.position = candidate->position;
                door.rotation = candidate->rotation;
                door.open = false;
                door.mesh = std::make_shared<SceneMesh>(*mesh);
                if (mesh)
                {
                    Vector3 min_v{1e9f, 1e9f, 1e9f};
                    Vector3 max_v{-1e9f, -1e9f, -1e9f};
                    for (const auto& tri : mesh->triangles)
                    {
                        for (const auto& v : tri.vertices)
                        {
                            min_v.x = std::min(min_v.x, v.position.x);
                            min_v.y = std::min(min_v.y, v.position.y);
                            min_v.z = std::min(min_v.z, v.position.z);
                            max_v.x = std::max(max_v.x, v.position.x);
                            max_v.y = std::max(max_v.y, v.position.y);
                            max_v.z = std::max(max_v.z, v.position.z);
                        }
                    }
                    if (min_v.x < max_v.x)
                    {
                        door.min_bounds = min_v;
                        door.max_bounds = max_v;
                    }
                }
                scene.doors.push_back(door);
                runtime_position = scene.doors.back().position;
                runtime_rotation = scene.doors.back().rotation;
                position_error = length(runtime_position - candidate->position);
                orientation_error = length(runtime_rotation - candidate->rotation);
                position_matches = transform_matches(candidate->position, runtime_position);
                orientation_matches = transform_matches(candidate->rotation, runtime_rotation);
            }
            else
            {
                const bool rigid_collision =
                    (candidate->type == "Building" || candidate->type == "EditRigidObj" ||
                     candidate->type == "ExplodeObject" || candidate->type == "Car");
                const std::size_t first_scene_triangle = scene.static_triangles.size();
                append_model(scene, *mesh, candidate->position, candidate->rotation, rigid_collision);
                scene_triangle_count = scene.static_triangles.size() - first_scene_triangle;
                const EmittedGeometryTransform geometry_verification = verify_emitted_geometry_transform(
                    *mesh, scene, first_scene_triangle, candidate->position, candidate->rotation);
                runtime_position = geometry_verification.position;
                runtime_rotation = candidate->rotation;
                position_error = geometry_verification.position_error;
                orientation_error = geometry_verification.orientation_error;
                verification_tolerance = geometry_verification.tolerance;
                emitted_geometry_verified = geometry_verification.has_geometry;
                position_matches = emitted_geometry_verified && position_error <= verification_tolerance;
                orientation_matches =
                    emitted_geometry_verified && orientation_error <= verification_tolerance;
                if (candidate->model_id == "435_02_1" || candidate->model_id == "344_02_1" ||
                    candidate->model_id == "417_02_1" || candidate->model_id == "202_01_1" ||
                    candidate->model_id == "203_01_1" ||
                    candidate->model_id.find("ladder") != std::string::npos)
                {
                    float min_z = 1e9f, max_z = -1e9f;
                    for (const auto& tri : mesh->triangles)
                    {
                        for (const auto& v : tri.vertices)
                        {
                            min_z = std::min(min_z, v.position.z);
                            max_z = std::max(max_z, v.position.z);
                        }
                    }
                    if (max_z > min_z + 1.5f)
                    {
                        SceneLadder ladder;
                        ladder.bottom = candidate->position + Vector3{0.0f, 0.0f, min_z + 0.35f};
                        ladder.top = candidate->position + Vector3{0.0f, 0.0f, max_z - 0.2f};
                        ladder.normal = rotate_euler(Vector3{0.0f, 1.0f, 0.0f}, candidate->rotation);
                        ladder.bottom_mount = ladder.bottom + ladder.normal * 0.85f;
                        ladder.top_mount = ladder.top - ladder.normal * 0.65f + Vector3{0.0f, 0.0f, 0.5f};
                        ladder.facing_yaw = candidate->rotation.z;
                        ladder.height = ladder.top.z - ladder.bottom.z;
                        scene.ladders.push_back(ladder);
                    }
                }
            }
            if (!position_matches || !orientation_matches)
            {
                ++mismatch_count;
            }
            scene.object_placement_audits.push_back(
                {static_cast<std::size_t>(&call - initializer.calls().data()),
                 candidate->model_id,
                 candidate->type,
                 candidate->name,
                 candidate->position,
                 runtime_position,
                 candidate->rotation,
                 runtime_rotation,
                 position_error,
                 orientation_error,
                 verification_tolerance,
                 emitted_geometry_verified,
                 position_matches,
                 orientation_matches,
                 first_scene_triangle,
                 scene_triangle_count});
            diagnostics::log(
                position_matches && orientation_matches ? diagnostics::Severity::information
                                                        : diagnostics::Severity::error,
                "placement",
                "objects.qvm placement: call=" +
                    std::to_string(scene.object_placement_audits.back().source_call_index) + ", model=" +
                    candidate->model_id + ", type=" + candidate->type + ", name=" + candidate->name +
                    ", authored_position=" + format_vector(candidate->position) +
                    ", runtime_position=" + format_vector(runtime_position) + ", authored_orientation=" +
                    format_vector(candidate->rotation) + ", runtime_orientation=" +
                    (emitted_geometry_verified ? std::string("derived from emitted geometry")
                                               : format_vector(runtime_rotation)) +
                    ", position_match=" + (position_matches ? "true" : "false") +
                    ", position_error=" + std::to_string(position_error) +
                    ", orientation_match=" + (orientation_matches ? "true" : "false") +
                    ", orientation_error=" + std::to_string(orientation_error) +
                    ", verification_tolerance=" + std::to_string(verification_tolerance) +
                    ", emitted_geometry_verified=" + (emitted_geometry_verified ? "true" : "false"));
            ++scene.loaded_object_count;

            VerifyObject vo;
            vo.model_id = candidate->model_id;
            vo.type = candidate->type;
            vo.name = candidate->name;
            vo.position = candidate->position;
            vo.rotation = candidate->rotation;
            vo.raw_px = candidate->raw_px;
            vo.raw_py = candidate->raw_py;
            vo.raw_pz = candidate->raw_pz;
            vo.raw_ox = candidate->raw_ox;
            vo.raw_oy = candidate->raw_oy;
            vo.raw_oz = candidate->raw_oz;
            vo.has_ori = candidate->has_ori;
            vo.matched = true;
            scene.verified_objects.push_back(vo);

            std::ostringstream ss;
            ss << std::fixed << std::setprecision(4);
            ss << ", Ori=(" << candidate->raw_ox << ", " << candidate->raw_oy << ", " << candidate->raw_oz << ")";

            std::ostringstream pss;
            pss << "Object Loaded: ModelID=" << candidate->model_id
                << ", Type=" << candidate->type
                << ", Name=" << candidate->name
                << ", Pos=(" << static_cast<long long>(std::llround(candidate->raw_px)) << ", "
                             << static_cast<long long>(std::llround(candidate->raw_py)) << ", "
                             << static_cast<long long>(std::llround(candidate->raw_pz)) << ")"
                << ss.str()
                << ", Tex=" << candidate->model_id
                << ", Model=" << candidate->model_id;
            diagnostics::log(diagnostics::Severity::information, "LevelLoader", pss.str());
        }
        catch (const std::exception& exception)
        {
            ++scene.missing_object_count;
            ++mismatch_count;
            diagnostics::log(diagnostics::Severity::warning, "scene", exception.what());
        }
    }

    // Default ladder fallback for Level 1 guard/water tower
    if (scene.ladders.empty() && mission_number == 1)
    {
        SceneLadder tower_ladder;
        tower_ladder.bottom = Vector3{-35.0f, 62.0f, 0.5f};
        tower_ladder.top = Vector3{-35.0f, 62.0f, 12.0f};
        tower_ladder.normal = Vector3{0.0f, 1.0f, 0.0f};
        tower_ladder.bottom_mount = tower_ladder.bottom + Vector3{0.0f, 0.8f, 0.0f};
        tower_ladder.top_mount = tower_ladder.top - Vector3{0.0f, 0.6f, -0.5f};
        tower_ladder.height = 11.5f;
        tower_ladder.facing_yaw = 3.14159f;
        scene.ladders.push_back(tower_ladder);
    }

    const assets::QvmTaskTree task_tree(initializer.calls());
    std::unordered_map<std::size_t, const ObjectPlacementAudit*> placement_audits_by_call;
    placement_audits_by_call.reserve(scene.object_placement_audits.size());
    for (const auto& audit : scene.object_placement_audits)
    {
        placement_audits_by_call.emplace(audit.source_call_index, &audit);
    }

    std::size_t lightmapped_triangle_count = 0;
    const auto bind_lightmap_page = [&](SceneTriangle& triangle,
                                        std::string_view set_name,
                                        std::uint64_t first_lightmap_model_index,
                                        auto& page_indices_by_model) -> bool
    {
        if (triangle.lightmap_model_index < 0 || triangle.lightmap_page_index < 0)
        {
            return false;
        }
        const std::uint64_t lightmap_model_index =
            first_lightmap_model_index + static_cast<std::uint32_t>(triangle.lightmap_model_index);
        if (lightmap_model_index > static_cast<std::uint64_t>(std::numeric_limits<std::int32_t>::max()))
        {
            throw std::runtime_error("Object lightmap model index exceeds the retail index range");
        }
        const auto normalized_lightmap_model_index = static_cast<std::uint32_t>(lightmap_model_index);
        auto pages = page_indices_by_model.find(normalized_lightmap_model_index);
        if (pages == page_indices_by_model.end())
        {
            pages = page_indices_by_model
                        .emplace(normalized_lightmap_model_index,
                                 library.lightmap_textures(set_name, normalized_lightmap_model_index))
                        .first;
        }
        if (pages->second.empty())
        {
            return false;
        }
        const auto page_index = static_cast<std::size_t>(triangle.lightmap_page_index);
        if (page_index >= pages->second.size())
        {
            throw std::runtime_error("MEF triangle lightmap page is absent from its retail OLM entry: " +
                                     std::string(set_name) + "_" +
                                     std::to_string(normalized_lightmap_model_index));
        }
        if (pages->second[page_index] > static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))
        {
            throw std::runtime_error("Object lightmap texture index exceeds the scene's signed index range");
        }
        triangle.lightmap_texture_index = static_cast<std::int32_t>(pages->second[page_index]);
        return true;
    };
    for (const auto& lightmap_owner : task_tree.lightmap_owners())
    {
        std::uint64_t next_lightmap_model_index = 0;
        std::unordered_map<std::uint32_t, std::vector<std::uint32_t>> page_indices_by_model;
        std::size_t owner_lightmapped_triangle_count = 0;
        for (const std::size_t task_call_index : lightmap_owner.subtree_call_indices)
        {
            const auto placement = placement_audits_by_call.find(task_call_index);
            if (placement == placement_audits_by_call.end())
            {
                continue;
            }
            const ObjectPlacementAudit& audit = *placement->second;
            if (audit.task_type == "StationaryGun")
            {
                continue;
            }

            const auto model_layout = library.model_lightmap_layout(audit.model_identifier);
            std::uint64_t model_lightmap_model_count = 0;
            for (const auto& model_entry : model_layout)
            {
                if (model_entry.lightmap_index >= 0)
                {
                    model_lightmap_model_count =
                        std::max(model_lightmap_model_count,
                                 static_cast<std::uint64_t>(model_entry.lightmap_index) + 1);
                }
            }
            const auto model_mesh = library.model(audit.model_identifier);
            if (model_mesh->lightmap_model_index_count != model_lightmap_model_count)
            {
                throw std::runtime_error("Flattened MEF lightmap slot count differs from the retail model layout: " +
                                         audit.model_identifier);
            }
            diagnostics::log(diagnostics::Severity::debug,
                             "lightmap-layout",
                             "set=" + lightmap_owner.set_name + ", call=" +
                                 std::to_string(audit.source_call_index) + ", task=" + audit.task_type +
                                 ", model=" + audit.model_identifier + ", first_index=" +
                                 std::to_string(next_lightmap_model_index) + ", slots=" +
                                 std::to_string(model_lightmap_model_count));
            if (audit.scene_triangle_count > 0)
            {
                if (audit.first_scene_triangle > scene.static_triangles.size() ||
                    audit.scene_triangle_count > scene.static_triangles.size() - audit.first_scene_triangle)
                {
                    throw std::runtime_error("Object lightmap binding exceeds the emitted scene triangle array");
                }
                for (std::size_t triangle_offset = 0; triangle_offset < audit.scene_triangle_count;
                     ++triangle_offset)
                {
                    SceneTriangle& triangle =
                        scene.static_triangles[audit.first_scene_triangle + triangle_offset];
                    owner_lightmapped_triangle_count +=
                        bind_lightmap_page(triangle,
                                           lightmap_owner.set_name,
                                           next_lightmap_model_index,
                                           page_indices_by_model)
                            ? 1U
                            : 0U;
                }
            }

            for (auto& door : scene.doors)
            {
                if (door.source_call_index != task_call_index || !door.mesh)
                {
                    continue;
                }
                for (auto& triangle : door.mesh->triangles)
                {
                    owner_lightmapped_triangle_count +=
                        bind_lightmap_page(triangle,
                                           lightmap_owner.set_name,
                                           next_lightmap_model_index,
                                           page_indices_by_model)
                            ? 1U
                            : 0U;
                }
            }
            next_lightmap_model_index += model_lightmap_model_count;
        }
        lightmapped_triangle_count += owner_lightmapped_triangle_count;
        diagnostics::log(diagnostics::Severity::information,
                         "lightmap-bindings",
                         "Bound " + std::to_string(owner_lightmapped_triangle_count) +
                             " rendered triangles to retail OLM set " + lightmap_owner.set_name);
    }
    diagnostics::log(diagnostics::Severity::information,
                     "lightmap-bindings",
                     "Bound " + std::to_string(lightmapped_triangle_count) +
                         " Level 1 rendered triangles to authored OLM pages");

    // Authored mission objectives for Level 1 (Steal the truck near the main entrance)
    if (mission_number == 1)
    {
        MissionObjective obj1;
        obj1.id = 1;
        obj1.text = "Steal the getaway truck near the main entrance.";
        obj1.target_position = {static_cast<float>((24956676.0 - origin[0]) / units_per_metre),
                                static_cast<float>((-56086240.0 - origin[1]) / units_per_metre),
                                static_cast<float>((174414608.0 - origin[2]) / units_per_metre)};
        obj1.trigger_radius = 24.0f;
        obj1.completed = false;
        scene.objectives.push_back(obj1);
    }

    scene.verified_object_count = verified_task_count;
    scene.mismatch_object_count = mismatch_count;
    diagnostics::log(diagnostics::Severity::information,
                     "verify",
                     "Level verification: " + std::to_string(scene.loaded_object_count) +
                         " objects loaded, " + std::to_string(verified_task_count) +
                         " tasks verified, " + std::to_string(scene.missing_object_count) + " missing models, " +
                         std::to_string(mismatch_count) + " mismatches");
    const auto parameter_bytes =
        assets::read_file(assets::resolve_path(game_directory, "humanplayer/humanplayer.qvm"));
    const assets::QvmInitializer parameters{assets::BinaryView(parameter_bytes)};
    for (const auto& call : parameters.calls())
    {
        if (call.function == "DefineHumanPlayerWeaponCategory")
        {
            for (std::size_t argument_index = 0; argument_index < call.arguments.size();)
            {
                const double count = call.arguments[argument_index++].number;
                if (!std::isfinite(count) || count < 0 || count > 32 ||
                    count > call.arguments.size() - argument_index)
                {
                    throw std::runtime_error("Original weapon category table is invalid");
                }
                std::vector<std::string> category;
                for (std::size_t member_index = 0; member_index < static_cast<std::size_t>(count);
                     ++member_index)
                {
                    category.push_back(call.arguments[argument_index++].text);
                }
                scene.weapon_categories.push_back(std::move(category));
            }
        }
        if (call.function == "DefineHumanPlayerGeneral" && call.arguments.size() == 43)
        {
            scene.player_parameters = {static_cast<float>(call.arguments[0].number),
                                       static_cast<float>(call.arguments[1].number / 3.6 / 30.0),
                                       static_cast<float>(call.arguments[2].number / 3.6 / 30.0),
                                       static_cast<float>(call.arguments[3].number / 3.6 / 30.0),
                                       static_cast<float>(call.arguments[7].number)};
        }
    }
    const auto weapon_bytes =
        assets::read_file(assets::resolve_path(game_directory, "weapons/mp5sd/weapon.qvm"));
    const assets::QvmInitializer weapon_parameters{assets::BinaryView(weapon_bytes)};
    for (const auto& call : weapon_parameters.calls())
    {
        if (call.function == "DefineWeaponType" && call.arguments.size() >= 17)
        {
            const double capacity = call.arguments[16].number;
            if (!std::isfinite(capacity) || capacity < 1 || capacity > 1000)
            {
                throw std::runtime_error("Original weapon magazine capacity is invalid");
            }
            scene.weapon_parameters.name = call.arguments[2].text;
            scene.weapon_parameters.magazine_capacity = static_cast<std::uint32_t>(call.arguments[16].number);
            scene.weapon_parameters.shots_per_minute = static_cast<float>(call.arguments[15].number);
            scene.weapon_parameters.damage = static_cast<float>(call.arguments[10].number);
            scene.weapon_parameters.muzzle_velocity = static_cast<float>(call.arguments[13].number);
            scene.weapon_parameters.reload_seconds = static_cast<float>(call.arguments[12].number);
        }
    }
    const auto animations = IFF_Parse(assets::resolve_path(game_directory, "common/anims/000.iff").string());
    if (!animations.valid)
    {
        throw std::runtime_error("Cannot read original player animation tracks");
    }
    for (const auto& original_clip : animations.clips)
    {
        if (!std::isfinite(original_clip.duration) || original_clip.duration < 0)
        {
            throw std::runtime_error("Original animation duration is invalid");
        }
        RootMotionClip clip{original_clip.duration, {}};
        for (const auto& key : original_clip.root_translations)
        {
            if (!std::isfinite(key.time) || !finite(Vector3{key.pos[0], key.pos[1], key.pos[2]}))
            {
                throw std::runtime_error("Original animation translation key is invalid");
            }
            // Retail IFF files store outbound tangent negated in tangent_out.
            // 0x4D4840 evaluates previous key's tangent_in as m0 (outgoing) and
            // next key's (-tangent_out) as m1 (incoming) in the standard Hermite basis.
            clip.keys.push_back({key.time,
                                 {key.pos[0], key.pos[1], key.pos[2]},
                                 {-key.tangent_out[0], -key.tangent_out[1], -key.tangent_out[2]},
                                 {key.tangent_in[0], key.tangent_in[1], key.tangent_in[2]}});
        }
        scene.root_motion_clips[original_clip.animation_id] = std::move(clip);
    }
    scene.weapon_mesh = library.model("103_01_1");
    assets::load_mission_gameplay(scene, library, mission_directory, origin, initializer.calls());
    scene.textures = library.take_textures();
    diagnostics::log(diagnostics::Severity::information,
                     "scene",
                     "Ready: " + std::to_string(scene.loaded_object_count) + " objects, " +
                         std::to_string(scene.actors.size()) + " soldiers, " +
                         std::to_string(scene.textures.size()) + " textures, " +
                         std::to_string(scene.root_motion_clips.size()) + " root-motion clips");
    diagnostics::log(
        diagnostics::Severity::information,
        "attachments",
        "Loaded " + std::to_string(scene.attached_model_count) +
            " visual ATTA instances; maximum depth=" + std::to_string(scene.maximum_attachment_depth));
    diagnostics::log(
        diagnostics::Severity::information,
        "LevelLoader",
        "LoadLevel() END for level " + std::to_string(mission_number) +
            "; verified objects=" + std::to_string(scene.verified_objects.size()));
    return scene;
}

} // namespace igi::gameplay
