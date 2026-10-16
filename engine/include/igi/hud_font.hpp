#pragma once

#include <cstdint>
#include <string>

namespace igi::gameplay {

// Portable, self-contained bitmap font renderer for the HUD.
// Uses an embedded 8x8 monospace glyph set (ASCII 32..126).
class HudFont final {
public:
    static constexpr int kBaseWidth = 8;
    static constexpr int kBaseHeight = 8;

    static void draw_char(
        std::uint32_t* buffer,
        int width,
        int height,
        int x,
        int y,
        char c,
        std::uint32_t color,
        int scale = 1) noexcept;

    static void draw_string(
        std::uint32_t* buffer,
        int width,
        int height,
        int x,
        int y,
        const std::string& text,
        std::uint32_t color,
        int scale = 1,
        bool with_shadow = true) noexcept;

    [[nodiscard]] static int string_width(const std::string& text, int scale = 1) noexcept;
    [[nodiscard]] static int line_height(int scale = 1) noexcept;
};

} // namespace igi::gameplay
