#pragma once

namespace igi::developer
{

// Host inspection controls requested by the owner; these are not retail native routines.
enum class DeveloperAction
{
    none,
    toggle_player_cheats,
    next_level,
    toggle_free_camera
};

struct DeveloperControls
{
    bool enabled = false;
    bool player_god_mode_requested = false;
    bool player_unlimited_ammunition_requested = false;
    bool player_unlimited_clips_requested = false;
    bool free_camera_enabled = false;

    DeveloperAction handle_key(unsigned int virtual_key, bool repeated)
    {
        if (!enabled || repeated)
        {
            return DeveloperAction::none;
        }
        if (virtual_key == 0x78U)
        {
            player_god_mode_requested = !player_god_mode_requested;
            player_unlimited_ammunition_requested = player_god_mode_requested;
            player_unlimited_clips_requested = player_god_mode_requested;
            return DeveloperAction::toggle_player_cheats;
        }
        if (virtual_key == 0x79U)
        {
            return DeveloperAction::next_level;
        }
        if (virtual_key == 0x7BU)
        {
            free_camera_enabled = !free_camera_enabled;
            return DeveloperAction::toggle_free_camera;
        }
        return DeveloperAction::none;
    }
};

} // namespace igi::developer
