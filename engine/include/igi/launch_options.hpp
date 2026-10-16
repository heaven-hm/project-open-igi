#pragma once

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

namespace igi::gameplay {

// Portable launch options: no OS APIs. The platform backend parses argv into
// this struct; gameplay/rendering code only reads the resolved values.
enum class GraphicsQuality : std::uint8_t {
    Low,
    High,
};

struct AppLaunchOptions {
    std::string game_root{"/Users/haseeb-mir/Downloads/IGI"};
    int mission_number{1};
    GraphicsQuality graphics_quality{GraphicsQuality::High};
    std::uint32_t render_width{960};
    std::uint32_t render_height{600};

    [[nodiscard]] static AppLaunchOptions parse(
        const std::vector<std::string>& arguments) noexcept {
        AppLaunchOptions options;
        bool explicit_size = false;
        for (std::size_t index = 0; index < arguments.size(); ++index) {
            const std::string& arg = arguments[index];
            auto next_value = [&](std::string& out) {
                if (index + 1 < arguments.size()) {
                    out = arguments[++index];
                }
            };
            if (arg == "--gfx" || arg == "--graphics") {
                std::string value;
                next_value(value);
                for (auto& c : value) c = static_cast<char>(std::tolower(c));
                if (value == "low") {
                    options.graphics_quality = GraphicsQuality::Low;
                    if (!explicit_size) {
                        options.render_width = 480;
                        options.render_height = 300;
                    }
                } else if (value == "high") {
                    options.graphics_quality = GraphicsQuality::High;
                    if (!explicit_size) {
                        options.render_width = 960;
                        options.render_height = 600;
                    }
                }
            } else if (arg == "--width") {
                std::string value;
                next_value(value);
                const int parsed = std::atoi(value.c_str());
                if (parsed >= 160 && parsed <= 3840) {
                    options.render_width = static_cast<std::uint32_t>(parsed);
                    explicit_size = true;
                }
            } else if (arg == "--height") {
                std::string value;
                next_value(value);
                const int parsed = std::atoi(value.c_str());
                if (parsed >= 100 && parsed <= 2160) {
                    options.render_height = static_cast<std::uint32_t>(parsed);
                    explicit_size = true;
                }
            } else if (arg == "--mission" || arg == "-m") {
                std::string value;
                next_value(value);
                const int parsed = std::atoi(value.c_str());
                if (parsed >= 1 && parsed <= 14) {
                    options.mission_number = parsed;
                }
            } else if (arg == "--game-root" || arg == "--game") {
                next_value(options.game_root);
            } else if (!arg.empty() && arg[0] != '-') {
                // Positional: first bare arg is game root or mission number.
                bool is_number = true;
                for (char c : arg) {
                    if (!std::isdigit(static_cast<unsigned char>(c))) {
                        is_number = false;
                        break;
                    }
                }
                if (is_number) {
                    const int parsed = std::atoi(arg.c_str());
                    if (parsed >= 1 && parsed <= 14) {
                        options.mission_number = parsed;
                    }
                } else {
                    options.game_root = arg;
                }
            }
        }
        return options;
    }
};

} // namespace igi::gameplay
