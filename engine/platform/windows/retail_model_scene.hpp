#pragma once
#include <windows.h>
#include <filesystem>

// Runs the Level <level_number> model scene from retail resources.
//
// dev_mode enables the host free camera: W/A/S/D move, Space raises, Shift lowers, and the mouse
// looks. It is host tooling for inspecting the loaded scene and is not a retail feature; the
// retail debug camera is Human_FindNextCamera (VA 0x004828D0), which needs the retail input and
// camera state that the port does not have yet.
int RunRetailModelScene(HINSTANCE instance, int show_command, const std::filesystem::path &game_directory,
                        unsigned int level_number, const std::filesystem::path &capture_path = {},
                        bool dev_mode = false, bool windowed = false);
