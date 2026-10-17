#include <iostream>
#include <stdexcept>

#include "../../platform/windows/developer_controls.hpp"

int main()
{
    try
    {
        using igi::developer::DeveloperAction;
        igi::developer::DeveloperControls controls;
        for (unsigned int key : {0x78U, 0x79U, 0x7BU})
        {
            if (controls.handle_key(key, false) != DeveloperAction::none)
            {
                throw std::runtime_error("Developer key active without devmode");
            }
        }
        controls.enabled = true;
        if (controls.handle_key(0x78U, true) != DeveloperAction::none || controls.player_god_mode_requested)
        {
            throw std::runtime_error("Repeated F9 changed state");
        }
        if (controls.handle_key(0x78U, false) != DeveloperAction::toggle_player_cheats ||
            !controls.player_god_mode_requested || !controls.player_unlimited_ammunition_requested ||
            !controls.player_unlimited_clips_requested)
        {
            throw std::runtime_error("F9 did not request player cheats");
        }
        controls.handle_key(0x78U, false);
        if (controls.player_god_mode_requested || controls.player_unlimited_ammunition_requested ||
            controls.player_unlimited_clips_requested)
        {
            throw std::runtime_error("F9 did not disable player cheat requests");
        }
        if (controls.handle_key(0x79U, false) != DeveloperAction::next_level ||
            controls.handle_key(0x7BU, false) != DeveloperAction::toggle_free_camera ||
            !controls.free_camera_enabled)
        {
            throw std::runtime_error("F10/F12 mapping failed");
        }
        controls.handle_key(0x7BU, false);
        if (controls.free_camera_enabled || controls.handle_key(0x7AU, false) != DeveloperAction::none)
        {
            throw std::runtime_error("F12 disable or unrelated key failed");
        }
        std::cout << "Developer controls tests passed; gameplay cheat integration pending.\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << '\n';
        return 1;
    }
}
