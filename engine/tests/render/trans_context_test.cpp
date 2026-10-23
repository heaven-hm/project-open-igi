#include "igi/retail/render/trans_context.hpp"
#include "igi/retail/render/view_block.hpp"
#include "igi/retail/human/human_view.hpp"

#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

// 0x00497E70 copies 0x2A dwords (0xA8 bytes) into the active context.
void test_setter_copies_the_whole_context()
{
    igi::retail::render::TransContext supplied{};
    for (std::size_t index = 0U; index < supplied.bytes.size(); ++index) {
        supplied.bytes[index] = static_cast<std::uint8_t>(index);
    }

    igi::retail::render::Transcontext_SetActiveTransContext(supplied);

    const igi::retail::render::TransContext& active =
        igi::retail::render::Transcontext_GetActiveTransContext();
    require(active.bytes.size() == 0xA8U, "context size is 0xA8 as rep movsd 0x2A implies");
    require(active.bytes == supplied.bytes, "the setter copies every byte of the context");
}

// The setter writes g_6e5bb0 = 1 and increments g_6e5bb4.
void test_setter_updates_the_retail_counters()
{
    const std::int32_t before = igi::retail::render::Transcontext_GetInstallCount();
    igi::retail::render::TransContext empty{};
    igi::retail::render::Transcontext_SetActiveTransContext(empty);
    require(igi::retail::render::Transcontext_GetInstalledFlag() == 1,
            "g_6e5bb0 is set to 1");
    require(igi::retail::render::Transcontext_GetInstallCount() == before + 1,
            "g_6e5bb4 is incremented");
}

// The camera position is the three float64 at +0x28 (0x00BCAB08/10/18).
void test_camera_position_maps_to_offset_0x28()
{
    igi::retail::render::Transcontext_SetCameraPosition(12.5, -3.25, 400.0);
    require(igi::retail::render::trans_context_camera_position_offset == 0x28U,
            "camera position offset is 0x28");

    const std::array<double, 3> position = igi::retail::render::Transcontext_GetCameraPosition();
    require(position[0] == 12.5 && position[1] == -3.25 && position[2] == 400.0,
            "camera position round-trips through +0x28");

    // The setter must carry the position across when a new context is installed.
    igi::retail::render::TransContext supplied{};
    const std::array<double, 3> moved{1.0, 2.0, 3.0};
    std::memcpy(supplied.bytes.data() + 0x28, moved.data(), sizeof(moved));
    igi::retail::render::Transcontext_SetActiveTransContext(supplied);
    const std::array<double, 3> active = igi::retail::render::Transcontext_GetCameraPosition();
    require(active[0] == 1.0 && active[1] == 2.0 && active[2] == 3.0,
            "installing a context installs its camera position");
}

// The default player view is reset by the retail Human_TaskViewReset native, then the
// retail view-block builder derives the vertical tangent from the active display aspect.
void test_default_player_view_builds_aspect_correct_fov()
{
    std::array<std::uint8_t, 0x1F0> human{};
    igi::retail::human::Human_TaskViewReset(human.data());
    float fov_x = 0.0F;
    std::memcpy(&fov_x, human.data() + igi::retail::human::human_view_tangent_x_offset,
                sizeof(fov_x));
    require(std::abs(fov_x - 1.0F) < 1.0e-6F,
            "Human_TaskViewReset supplies tan(45 degrees) for the normal player view");

    igi::retail::render::Display_HostSetDimensions(1024, 768);
    igi::retail::render::ScreenBlock_HostInstallRect(0.0F, 0.0F, 1024.0F, 768.0F);
    std::array<std::uint8_t, 0x28> view{};
    const std::array<double, 3> camera{0.0, 0.0, 0.0};
    igi::retail::render::TransContext context{};
    require(igi::retail::render::Transcontext_BuildViewBlock(context.bytes.data(), view.data(),
                                                              camera.data(), fov_x) == 1,
            "Transcontext_BuildViewBlock returns the retail success value");
    igi::retail::render::Transcontext_SetActiveTransContext(context);
    const auto fov = igi::retail::render::Transcontext_GetFovTangents();
    require(std::abs(fov[0] - 1.0F) < 1.0e-6F && std::abs(fov[1] - 0.75F) < 1.0e-6F,
            "active context stores retail horizontal and 4:3 vertical half-FOV tangents");
}

} // namespace

int main()
{
    test_setter_copies_the_whole_context();
    test_setter_updates_the_retail_counters();
    test_camera_position_maps_to_offset_0x28();
    test_default_player_view_builds_aspect_correct_fov();

    std::cout << "All retail transform-context tests passed.\n";
    return EXIT_SUCCESS;
}
