#include "test_framework.hpp"

#include "igi/launch_options.hpp"
#include "igi/mission_world.hpp"

using namespace igi::test;
using igi::gameplay::AppLaunchOptions;
using igi::gameplay::GraphicsQuality;
using igi::gameplay::TextureImage;

namespace {

TextureImage make_checker_2x2() {
    TextureImage image;
    image.name = "test_checker";
    image.width = 2;
    image.height = 2;
    image.has_alpha = false;
    // Row-major: (0,0)=black, (1,0)=red, (0,1)=green, (1,1)=white.
    image.argb_pixels = {
        0xFF000000u, 0xFFFF0000u,
        0xFF00FF00u, 0xFFFFFFFFu,
    };
    return image;
}

std::uint8_t channel_of(std::uint32_t argb, int shift) {
    return static_cast<std::uint8_t>((argb >> shift) & 0xFFu);
}

} // namespace

TEST_CASE(GraphicsQuality, BilinearCenterBlendsFourTexels) {
    const TextureImage image = make_checker_2x2();
    // UV center of the 2x2 image blends all four texels ~equally:
    // R=(0+255+0+255)/4=127.5, G=(0+0+255+255)/4=127.5, B=(0+0+0+255)/4=63.75.
    const std::uint32_t blended = image.sample_bilinear(0.5f, 0.5f);
    ASSERT_NEAR(static_cast<double>(channel_of(blended, 16)), 128.0, 24.0);
    ASSERT_NEAR(static_cast<double>(channel_of(blended, 8)), 128.0, 24.0);
    ASSERT_NEAR(static_cast<double>(channel_of(blended, 0)), 64.0, 24.0);
}

TEST_CASE(GraphicsQuality, BilinearMatchesNearestAtTexelCenters) {
    const TextureImage image = make_checker_2x2();
    // Texel (1,0) center is at u=0.75, v=0.25 in a 2x2 image.
    const std::uint32_t nearest = image.sample_nearest(0.75f, 0.25f);
    const std::uint32_t blended = image.sample_bilinear(0.75f, 0.25f);
    ASSERT_TRUE(nearest == 0xFFFF0000u);
    ASSERT_NEAR(static_cast<double>(channel_of(blended, 16)), 255.0, 16.0);
    ASSERT_TRUE(channel_of(blended, 8) < 32u);
    ASSERT_TRUE(channel_of(blended, 0) < 32u);
}

TEST_CASE(GraphicsQuality, BilinearWrapsLikeNearest) {
    const TextureImage image = make_checker_2x2();
    const std::uint32_t a = image.sample_bilinear(1.5f, 0.5f);
    const std::uint32_t b = image.sample_bilinear(0.5f, 0.5f);
    ASSERT_TRUE(a == b);
}

TEST_CASE(GraphicsQuality, LaunchDefaultsToHigh960x600) {
    const AppLaunchOptions options = AppLaunchOptions::parse({});
    ASSERT_TRUE(options.graphics_quality == GraphicsQuality::High);
    ASSERT_EQ(options.render_width, 960u);
    ASSERT_EQ(options.render_height, 600u);
    ASSERT_EQ(options.mission_number, 1);
}

TEST_CASE(GraphicsQuality, LaunchLowFlagSelects480x300) {
    const AppLaunchOptions options =
        AppLaunchOptions::parse({"--gfx", "low"});
    ASSERT_TRUE(options.graphics_quality == GraphicsQuality::Low);
    ASSERT_EQ(options.render_width, 480u);
    ASSERT_EQ(options.render_height, 300u);
}

TEST_CASE(GraphicsQuality, LaunchExplicitSizeSurvivesQualityFlag) {
    const AppLaunchOptions options = AppLaunchOptions::parse(
        {"--width", "800", "--height", "500", "--gfx", "low"});
    ASSERT_TRUE(options.graphics_quality == GraphicsQuality::Low);
    ASSERT_EQ(options.render_width, 800u);
    ASSERT_EQ(options.render_height, 500u);
}

TEST_CASE(GraphicsQuality, LaunchMissionAndRootForms) {
    const AppLaunchOptions by_flag =
        AppLaunchOptions::parse({"--mission", "2"});
    ASSERT_EQ(by_flag.mission_number, 2);
    const AppLaunchOptions positional =
        AppLaunchOptions::parse({"2"});
    ASSERT_EQ(positional.mission_number, 2);
    const AppLaunchOptions root =
        AppLaunchOptions::parse({"/tmp/IGI", "--gfx", "high"});
    ASSERT_TRUE(root.game_root == "/tmp/IGI");
    ASSERT_TRUE(root.graphics_quality == GraphicsQuality::High);
}
