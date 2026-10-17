#include "igi/retail/render/camera_transform.hpp"

#include <cstdint>
#include <cstdlib>
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

// 0x004B7450 writes 0 at matrix + slot*4 + 0x14 and returns the slot index.
void test_push_stack_clears_the_slot_at_0x14_plus_index()
{
    std::int32_t matrix[64];
    for (std::int32_t& value : matrix) {
        value = 0x5A5A5A5A;
    }

    const std::int32_t returned = igi::retail::render::Sys_PushStack_19(matrix, 3);

    require(returned == 3, "returns the slot index");
    require(matrix[(0x14 / 4) + 3] == 0, "clears matrix + 3*4 + 0x14");
    require(matrix[(0x14 / 4) + 2] == 0x5A5A5A5A, "leaves the neighbouring slot untouched");
    require(matrix[(0x14 / 4) + 4] == 0x5A5A5A5A, "leaves the next slot untouched");
}

// 0x004A0110 invalidates the eight camera slots in its recorded call order.
void test_camera_update_transform_clears_all_eight_slots()
{
    igi::retail::render::CameraMatrixState& state =
        igi::retail::render::Camera_GetMatrixState();

    for (std::int32_t& value : state.slab_bcad7c) { value = -1; }
    for (std::int32_t& value : state.slab_bcadac) { value = -1; }
    for (std::int32_t& value : state.slab_bcade8) { value = -1; }

    state.slot_7205b8 = 1;
    state.slot_723508 = 2;
    state.slot_7205bc = 3;
    state.slot_723504 = 4;
    state.slot_7addd4 = 5;
    state.slot_7205b0 = 6;
    state.slot_7ac09c = 7;
    state.slot_7234b8 = 8;

    igi::retail::render::Camera_UpdateTransform();

    const std::size_t base = igi::retail::render::camera_matrix_slot_base / 4U;
    require(state.slab_bcad7c[base + 1] == 0, "slab 0xBCAD7C slot 1 cleared");
    require(state.slab_bcad7c[base + 7] == 0, "slab 0xBCAD7C slot 7 cleared");
    require(state.slab_bcadac[base + 2] == 0, "slab 0xBCADAC slot 2 cleared");
    require(state.slab_bcadac[base + 3] == 0, "slab 0xBCADAC slot 3 cleared");
    require(state.slab_bcadac[base + 5] == 0, "slab 0xBCADAC slot 5 cleared");
    require(state.slab_bcadac[base + 8] == 0, "slab 0xBCADAC slot 8 cleared");
    require(state.slab_bcade8[base + 4] == 0, "slab 0xBCADE8 slot 4 cleared");
    require(state.slab_bcade8[base + 6] == 0, "slab 0xBCADE8 slot 6 cleared");
    require(state.slab_bcade8[base + 9] == -1, "slots outside the list stay untouched");
}

// 0x004A26C0 packs RGB into 0x00RRGGBB and fills the 256-entry palette.
void test_camera_update_transform_2_packs_color_and_fills_palette()
{
    igi::retail::render::CameraMatrixState& state =
        igi::retail::render::Camera_GetMatrixState();

    state.palette_935c10.fill(0);

    igi::retail::render::CameraColorContext context{};
    context.color_r = 1.0f;
    context.color_g = 0.5f;
    context.color_b = 0.25f;

    const std::int32_t returned = igi::retail::render::Camera_UpdateTransform_2(&context);

    // Expected value:
    // r_scaled = round(-255.0f * 1.0f) = -255
    // g_scaled = round(-255.0f * 0.5f) = -128
    // b_scaled = round(255.0f * 0.25f) = 64
    // ecx = ((-255 << 8) + -128) << 8 = -65408 << 8 = -16744448
    // color = 64 - (-16744448) = 16744512 = 0x00FF8040
    constexpr std::int32_t expected_color = 0x00FF8040;
    require(returned == expected_color, "Camera_UpdateTransform_2 returns expected packed color");

    for (std::size_t i = 0; i < igi::retail::render::camera_palette_entry_count; ++i) {
        require(state.palette_935c10[i] == static_cast<std::uint32_t>(expected_color),
                "palette_935c10 entry matches packed color");
    }

    require(igi::retail::render::Camera_UpdateTransform_2(nullptr) == 0,
            "null context safely returns 0");
}

// 0x004A2600 sets 4 alternating 32-dword bands of half- and full-intensity color.
void test_camera_reset_matrix_fills_banded_palette()
{
    igi::retail::render::CameraMatrixState& state =
        igi::retail::render::Camera_GetMatrixState();

    state.palette_935c10.fill(0);

    igi::retail::render::CameraColorContext context{};
    context.color_r = 1.0f;
    context.color_g = 1.0f;
    context.color_b = 1.0f;

    const std::int32_t returned = igi::retail::render::Camera_ResetMatrix(&context);

    // Half: round(-128 * 1) = -128, round(128 * 1) = 128 -> 128 - ((-128 << 8) + -128) << 8 = 0x00808080
    // Full: round(-255 * 1) = -255, round(255 * 1) = 255 -> 255 - ((-255 << 8) + -255) << 8 = 0x00FFFFFF
    constexpr std::uint32_t expected_half = 0x00808080U;
    constexpr std::uint32_t expected_full = 0x00FFFFFFU;

    require(returned == static_cast<std::int32_t>(expected_full),
            "Camera_ResetMatrix returns full-intensity color");

    std::size_t idx = 0;
    for (std::size_t band = 0; band < 4; ++band) {
        for (std::size_t i = 0; i < 0x20; ++i) {
            require(state.palette_935c10[idx++] == expected_half,
                    "banded palette half-intensity entry matches");
        }
        for (std::size_t i = 0; i < 0x20; ++i) {
            require(state.palette_935c10[idx++] == expected_full,
                    "banded palette full-intensity entry matches");
        }
    }

    require(idx == igi::retail::render::camera_palette_entry_count,
            "all 256 palette entries populated");
    require(igi::retail::render::Camera_ResetMatrix(nullptr) == 0,
            "null context safely returns 0");
}

} // namespace

int main()
{
    test_push_stack_clears_the_slot_at_0x14_plus_index();
    test_camera_update_transform_clears_all_eight_slots();
    test_camera_update_transform_2_packs_color_and_fills_palette();
    test_camera_reset_matrix_fills_banded_palette();

    std::cout << "All retail camera-transform tests passed.\n";
    return EXIT_SUCCESS;
}

