#pragma once

#include "igi/vector_math.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace igi::gameplay
{

enum class TerrainDetail
{
    low,
    medium,
    high,
    enhanced,
    ultra
};

struct SceneLadder final
{
    Vector3 bottom{};
    Vector3 top{};
    Vector3 bottom_mount{};
    Vector3 top_mount{};
    Vector3 normal{0.0f, 1.0f, 0.0f};
    float facing_yaw{};
    float height{};
};

struct MissionObjective final
{
    int id = 1;
    std::string text = "Steal the truck near the main entrance.";
    Vector3 target_position{};
    float trigger_radius = 16.0f;
    bool completed = false;
};

struct VerifyObject final
{
    std::string model_id;
    std::string type;
    std::string name;
    Vector3 position{};
    Vector3 rotation{};
    bool matched = false;
};

struct SceneTextureMip final
{
    std::uint32_t width{};
    std::uint32_t height{};
    std::vector<std::uint32_t> pixels;
    bool authored{};
};

struct SceneTexture final
{
    std::uint32_t width{};
    std::uint32_t height{};
    std::vector<std::uint32_t> pixels;
    std::vector<SceneTextureMip> mipmaps;
};

struct SceneFogParameters final
{
    Vector3 colour{0.0f, 0.0f, 0.0f};
    float density_per_metre{};
};

struct SceneVertex final
{
    Vector3 position;
    float texture_u{};
    float texture_v{};
    std::uint32_t bone_index = static_cast<std::uint32_t>(-1);
    Vector3 bone_local_position{};
    float lightmap_u{};
    float lightmap_v{};
};

struct SceneTriangle final
{
    std::array<SceneVertex, 3> vertices;
    std::uint32_t texture_index{};
    bool firing_only{};
    std::int32_t lightmap_page_index = -1;
    std::int32_t lightmap_texture_index = -1;
    std::int32_t lightmap_model_index = -1;
};

struct SceneSkinTriangle final
{
    std::array<SceneVertex, 3> vertices;
};

struct CollisionTriangle final
{
    std::array<Vector3, 3> positions;
    bool non_solid_portal{};
};

struct SceneMesh final
{
    struct ModelPart final
    {
        std::string model_name;
        std::size_t first_triangle{};
        std::size_t triangle_count{};
        std::int32_t lightmap_model_index = -1;
    };

    std::vector<SceneTriangle> triangles;
    std::vector<ModelPart> model_parts;
    std::vector<SceneSkinTriangle> skinned_collision;
    std::vector<CollisionTriangle> collision;
    std::vector<Vector3> bone_rest_positions;
    std::size_t attachment_count{};
    std::size_t maximum_attachment_depth{};
    std::uint32_t lightmap_model_index_count{};
    bool is_sector{};
    std::vector<CollisionTriangle> movement_collision;
};

struct SceneDoor final
{
    int task_id{};
    std::size_t source_call_index = static_cast<std::size_t>(-1);
    std::string name;
    std::string model_id;
    Vector3 position{};
    Vector3 rotation{};
    bool open = false;
    float swing_angle = 1.5707963f;
    Vector3 min_bounds{-0.2f, -0.2f, 0.0f};
    Vector3 max_bounds{1.4f, 0.2f, 2.5f};
    std::size_t collision_start{};
    std::size_t collision_count{};
    std::shared_ptr<SceneMesh> mesh;
};

struct SkeletalAnimationSet;

struct SceneActor final
{
    Vector3 position;
    float heading{};
    int identifier{};
    int team = 1;
    std::shared_ptr<SceneMesh> mesh;
    int navigation_graph_identifier = -1;
    int patrol_path_identifier = -1;
    std::string weapon_identifier = "WEAPON_ID_AK47";
    std::size_t source_call_index = static_cast<std::size_t>(-1);
    bool gameplay_enabled = true;
    int skeleton_identifier = 1;
    int stand_animation = 2;
    std::shared_ptr<SkeletalAnimationSet> animations{};
    float maximum_health = 1.0f;
    float hit_probability_at_minimum_range = 1.0f;
    float hit_probability_at_maximum_range = 0.25f;
    float aim_precision = 0.2f;
    float primary_view_length = 26.0f;
    std::int32_t maximum_rounds_per_sequence = 30;
    float minimum_clip_fraction = 0.1f;
    float random_clip_fraction = 0.2f;
    float minimum_shot_delay_seconds = 0.1f;
    float random_shot_delay_seconds{};
};

struct NavigationNode final
{
    int identifier{};
    Vector3 position;
    std::vector<std::size_t> neighbors;
};

struct NavigationRouteEntry final
{
    int next_node_identifier = -1;
    float total_cost = -1.0f;
};

struct NavigationGraph final
{
    int identifier{};
    std::vector<NavigationNode> nodes;
    int route_table_width{};
    std::vector<NavigationRouteEntry> route_table;
};

struct PatrolCommand final
{
    int kind{};
    int operand{};
};

struct PatrolPath final
{
    int identifier{};
    std::vector<PatrolCommand> commands;
};

struct PlayerParameters final
{
    float root_translation_scale = 1.75f;
    float jump_horizontal_per_tick = 17.5f / 3.6f / 30.0f;
    float jump_vertical_per_tick = 27.0f / 3.6f / 30.0f;
    float airborne_control_per_tick = 0.5f / 3.6f / 30.0f;
    float maximum_health = 3.0f;
};

enum class WeaponCategory
{
    gun,
    pistol,
    shotgun,
    knife,
    grenade,
    binocular,
    medipack,
    proximity_mine
};

struct WeaponParameters final
{
    std::string name = "MP5 SD3";
    std::uint32_t magazine_capacity = 32;
    float shots_per_minute = 700.0f;
    float damage = 0.15f;
    float muzzle_velocity = 400.0f;
    float reload_seconds = 2.0f;
    float maximum_distance = 400.0f;
    bool automatic = true;
    bool melee{};
    WeaponCategory category = WeaponCategory::gun;
    std::int32_t maximum_rounds_per_burst = 32;
    std::int32_t projectiles_per_round = 1;
    float minimum_spread_degrees{};
    float maximum_spread_degrees{};
    float fixed_recoil_pitch_degrees{};
    float fixed_recoil_yaw_degrees{};
    float random_recoil_pitch_degrees{};
    float random_recoil_yaw_degrees{};
};

struct SceneWeapon final
{
    std::string identifier;
    WeaponParameters parameters;
    std::shared_ptr<SceneMesh> mesh;
    std::uint32_t initial_reserve{};
    bool initially_owned{};
    std::string ammunition_identifier;
    int stand_animation = 18;
    int move_animation = 19;
    int fire_animation = 20;
    int fire_animation_1 = 20;
    int fire_animation_2 = -1;
    int fire_animation_3 = -1;
    int reload_animation = 21;
};

struct WeaponPickup final
{
    Vector3 position;
    std::string weapon_identifier;
};

struct RootMotionKey final
{
    float time{};
    Vector3 position;
    Vector3 incoming_tangent;
    Vector3 outgoing_tangent;
};

struct RootMotionClip final
{
    float duration{};
    std::vector<RootMotionKey> keys;
};

struct ObjectPlacementAudit final
{
    std::size_t source_call_index{};
    std::string model_identifier;
    std::string task_type;
    std::string task_name;
    Vector3 authored_position;
    Vector3 runtime_position;
    Vector3 authored_rotation;
    Vector3 runtime_rotation;
    float position_error{};
    float orientation_error{};
    float verification_tolerance{};
    bool emitted_geometry_verified{};
    bool position_matches{};
    bool orientation_matches{};
    std::size_t first_scene_triangle{};
    std::size_t scene_triangle_count{};
};

struct MissionScene final
{
    std::vector<SceneTexture> textures;
    std::vector<SceneTriangle> static_triangles;
    std::vector<CollisionTriangle> collision_triangles;
    std::vector<SceneActor> actors;
    std::shared_ptr<SceneMesh> weapon_mesh;
    PlayerParameters player_parameters;
    WeaponParameters weapon_parameters;
    std::uint32_t initial_reserve_ammunition = 196;
    std::unordered_map<std::uint32_t, RootMotionClip> root_motion_clips;
    float player_heading{};
    std::size_t loaded_object_count{};
    std::size_t missing_object_count{};
    std::filesystem::path game_directory;
    std::vector<NavigationGraph> navigation_graphs;
    std::vector<PatrolPath> patrol_paths;
    std::vector<SceneWeapon> weapons;
    std::vector<WeaponPickup> weapon_pickups;
    int mission_number = 1;
    std::vector<std::vector<std::string>> weapon_categories;
    std::size_t attached_model_count{};
    std::size_t maximum_attachment_depth{};
    std::shared_ptr<SceneMesh> first_person_mesh{};
    std::shared_ptr<SkeletalAnimationSet> first_person_animations{};
    std::size_t terrain_triangle_count{};
    std::vector<CollisionTriangle> movement_collision_triangles;
    std::vector<SceneLadder> ladders;
    std::vector<SceneDoor> doors;
    std::vector<MissionObjective> objectives;
    std::vector<ObjectPlacementAudit> object_placement_audits;
    std::size_t verified_object_count{};
    std::size_t mismatch_object_count{};
    SceneFogParameters terrain_fog;
    SceneFogParameters object_fog;
};

[[nodiscard]] MissionScene load_mission(const std::filesystem::path& game_directory,
                                        const std::filesystem::path& cache_directory,
                                        int mission_number = 1,
                                        TerrainDetail terrain_detail = TerrainDetail::high);

} // namespace igi::gameplay
