#pragma once
#include <windows.h>
#include <filesystem>
int RunRetailModelScene(HINSTANCE instance, int show_command, const std::filesystem::path &game_directory,
                        unsigned int level_number, const std::filesystem::path &capture_path = {});
