#include "igi/skeletal_animation.hpp"

#include <exception>
#include <filesystem>
#include <iostream>
#include <numbers>
#include <stdexcept>

using namespace igi::gameplay;

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

void verify_original_animation_events(const std::filesystem::path& game_directory)
{
    const auto player_animations = load_skeletal_animations(game_directory / "common/ANIMS/000.IFF");
    const auto weapon_animations = load_skeletal_animations(game_directory / "common/ANIMS/005.IFF");
    const auto count_events = [](const SkeletalAnimationSet& animations)
    {
        std::size_t event_count = 0;
        for (const auto& [animation_identifier, clip] : animations.clips)
        {
            static_cast<void>(animation_identifier);
            event_count += clip.events.size();
        }
        return event_count;
    };

    require(player_animations->clips.size() == 122, "Retail player animation clip count changed");
    require(count_events(*player_animations) == 73, "Retail player animation events were dropped or duplicated");
    require(weapon_animations->clips.size() == 78, "Retail first-person animation clip count changed");
    require(count_events(*weapon_animations) == 52, "Retail weapon animation events were dropped or duplicated");

    const auto reload_clip = weapon_animations->clips.find(17);
    require(reload_clip != weapon_animations->clips.end(), "Retail Glock reload clip 17 is missing");
    require(reload_clip->second.events.size() == 3, "Retail Glock reload event count changed");
    require(reload_clip->second.events[0].event_identifier == 1000 &&
                reload_clip->second.events[1].event_identifier == 1001 &&
                reload_clip->second.events[2].event_identifier == 1002,
            "Retail Glock reload event identifiers changed");
    require(reload_clip->second.events[0].time == 2577.0F &&
                reload_clip->second.events[0].bone_identifier == 0 &&
                std::abs(reload_clip->second.events[0].position_components[0] - (-3.47418F)) < 0.0001F &&
                std::abs(reload_clip->second.events[0].position_components[1] - 51.8962F) < 0.0001F &&
                std::abs(reload_clip->second.events[0].position_components[2] - (-6.63869F)) < 0.0001F,
            "Retail Glock reload event time, bone, or position components were not preserved");

    const auto fence_walk_clip = player_animations->clips.find(171);
    require(fence_walk_clip != player_animations->clips.end(), "Retail fence-walk animation clip is missing");
    require(fence_walk_clip->second.events.size() == 10,
            "Retail fence-walk animation event count changed");
    require(fence_walk_clip->second.events.front().event_identifier == 1025 &&
                fence_walk_clip->second.events.back().event_identifier == 1026,
            "Retail fence-walk event ordering was not preserved");

    std::cout << "Retail animation event payloads passed: " << count_events(*player_animations)
              << " player events and " << count_events(*weapon_animations) << " first-person events\n";
}

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count > 2)
        {
            throw std::runtime_error("Usage: mvp_skeletal_probe [original-game-directory]");
        }
        const glm::quat identity{1, 0, 0, 0};
        const auto quarter_turn = glm::angleAxis(std::numbers::pi_v<float> * 0.5f, glm::vec3{0, 0, 1});
        const auto midpoint = interpolate_rotation(identity, quarter_turn, 0.5f);
        const auto rotated = midpoint * glm::vec3{1, 0, 0};
        require(std::abs(rotated.x - std::sqrt(0.5f)) < 0.00001f &&
                    std::abs(rotated.y - std::sqrt(0.5f)) < 0.00001f,
                "Quaternion interpolation must take the expected angular midpoint");
        require(glm::length(interpolate_rotation(identity, -quarter_turn, 0.5f) - midpoint) < 0.00001f,
                "Antipodal quaternion representation must preserve the shorter path");
        const std::vector<BoneRotationKey> keys{{0, identity, identity, identity},
                                                {160, quarter_turn, quarter_turn, quarter_turn}};
        require(glm::length(sample_bone_rotation(keys, 0, 320, false) - identity) == 0,
                "Exact key timestamps must return the stored key unchanged");
        require(glm::length(sample_bone_rotation(keys, 240, 320, false) - quarter_turn) == 0,
                "Non-cyclic rotation tracks must hold their last key");
        require(glm::length(sample_bone_rotation(keys, 240, 320, true) - midpoint) < 0.00001f,
                "Cyclic tracks must bridge the last key to the first");

        SkeletalAnimationSet animations;
        animations.parents = {-1, 0};
        animations.rest_offsets = {{0, 0, 0}, {1, 0, 0}};
        SkeletalAnimationClip clip;
        clip.duration = 320;
        clip.bone_rotations = {{{0, quarter_turn, quarter_turn, quarter_turn}}, {}};
        animations.clips[34] = clip;
        const auto pose = sample_bone_pose(animations, 34, 0);
        SkeletalAnimationSet death_animation;
        death_animation.parents = {-1};
        death_animation.rest_offsets = {{0, 0, 0}};
        SkeletalAnimationClip non_looping_death_clip;
        non_looping_death_clip.duration = 320;
        non_looping_death_clip.bone_rotations = {
            {{0, identity, identity, identity}, {160, quarter_turn, quarter_turn, quarter_turn}}};
        death_animation.clips[28] = non_looping_death_clip;
        const auto death_pose_at_start = sample_bone_pose(death_animation, 28, 0);
        const auto death_pose_at_end = sample_bone_pose(death_animation, 28, 320);
        require(std::abs(death_pose_at_start[0][0][0] - death_pose_at_end[0][0][0]) > 0.5F,
                "A completed retail death clip must hold its fallen pose instead of looping back to standing");
        SkeletalAnimationClip looping_death_clip = non_looping_death_clip;
        looping_death_clip.flags = 0x80000000u;
        const Vector3 root_tangent{0.0f, 0.0f, -4096.0f / 320.0f};
        looping_death_clip.root_motion = {
            320.0f,
            {{0.0f, {}, root_tangent, root_tangent},
             {320.0f, {0.0f, 0.0f, -4096.0f}, root_tangent, root_tangent}},
            {}};
        death_animation.clips[28] = looping_death_clip;
        const auto cyclic_death_root = sample_animation_root(death_animation, 28, 320.0f);
        const auto one_shot_death_root = sample_animation_root(death_animation, 28, 320.0f, true);
        require(std::abs(cyclic_death_root.z) < 0.00001f &&
                    std::abs(one_shot_death_root.z + 1.0f) < 0.00001f,
                "A dead soldier must sample the final root-motion key instead of wrapping a cyclic death clip");
        const auto one_shot_death_pose_at_start = sample_bone_pose(death_animation, 28, 0, false, true);
        const auto one_shot_death_pose_at_end = sample_bone_pose(death_animation, 28, 320, false, true);
        require(std::abs(one_shot_death_pose_at_start[0][0][0] - one_shot_death_pose_at_end[0][0][0]) > 0.5F,
                "A death state must override a cyclic asset flag and hold the final fallen pose");
        death_animation.clips[4] = looping_death_clip;
        const auto looping_pose_at_end = sample_bone_pose(death_animation, 4, 320);
        require(std::abs(death_pose_at_start[0][0][0] - looping_pose_at_end[0][0][0]) < 0.00001F,
                "A retail looping locomotion clip must wrap to its first pose");
        SceneVertex vertex;
        vertex.bone_index = 1;
        vertex.bone_local_position = {1, 0, 0};
        const auto position = skin_vertex(vertex, pose);
        require(length(position - Vector3{0, 2, 0}) < 0.00001f,
                "Skinning must compose parent transforms and the vertex's local bone offset");
        animations.parents[0] = 1;
        bool rejected = false;
        try
        {
            static_cast<void>(sample_bone_pose(animations, 34, 0));
        }
        catch (const std::runtime_error&)
        {
            rejected = true;
        }
        require(rejected, "Cyclic skeletons must fail through controlled error handling");
        if (argument_count == 2)
        {
            verify_original_animation_events(arguments[1]);
        }
        std::cout << "Skeletal probe passed: angular interpolation, antipodal signs, cyclic tracks, parent "
                     "transforms, skinning, cycle rejection\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Skeletal probe failed: " << exception.what() << '\n';
        return 1;
    }
}
