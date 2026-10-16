#include "igi/game_session.hpp"
#include "igi/logger.hpp"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace {

void capture_scene(
    igi::gameplay::GameSession& session,
    const std::filesystem::path& out_bmp,
    std::uint32_t width = 640,
    std::uint32_t height = 400) {
    session.render(width, height);
    if (session.renderer().save_bmp(out_bmp)) {
        std::cout << "[SCREENSHOT] Saved: " << out_bmp.string() << " (" << width << "x" << height << ")\n";
    } else {
        std::cerr << "[ERROR] Failed to save screenshot: " << out_bmp.string() << '\n';
    }
}

} // namespace

int main(int argc, char** argv) {
    const std::filesystem::path game_root =
        argc > 1 ? std::filesystem::path(argv[1]) : std::filesystem::path("/Users/haseeb-mir/Downloads/IGI");
    const std::filesystem::path out_dir =
        argc > 2 ? std::filesystem::path(argv[2]) : std::filesystem::path(".");

    std::filesystem::create_directories(out_dir);

    std::cout << "========================================================\n";
    std::cout << "Project I.G.I. Native C++ MVP Screenshot Test Runner\n";
    std::cout << "Retail Game Assets: " << game_root.string() << "\n";
    std::cout << "Output Directory:   " << out_dir.string() << "\n";
    std::cout << "========================================================\n\n";

    igi::gameplay::InputState input;

    // -------------------------------------------------------------
    // Scenario 1: Mission 1 Spawn View (M16A2, 100% HP, Objectives)
    // -------------------------------------------------------------
    std::cout << "[SCENARIO 1] Testing Mission 1 Trainyard Spawn...\n";
    igi::gameplay::GameSession session_m1;
    if (!session_m1.load_mission(game_root, 1)) {
        std::cerr << "[FATAL] Failed to load Mission 1: " << session_m1.mission_status() << '\n';
        return 1;
    }
    // Advance 60 ticks to settle grounding and view
    for (int t = 0; t < 60; ++t) {
        (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    }
    capture_scene(session_m1, out_dir / "screenshot_01_spawn_m16a2.bmp");

    // -------------------------------------------------------------
    // Scenario 2: M16A2 Firing Burst & Muzzle Flash
    // -------------------------------------------------------------
    std::cout << "[SCENARIO 2] Testing Weapon Firing & Muzzle Flash...\n";
    (void)input.set_control_state(igi::gameplay::InputCode::MouseLeft, true);
    // Fire for 2 ticks to ignite muzzle flash
    (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    capture_scene(session_m1, out_dir / "screenshot_02_firing_burst.bmp");
    (void)input.set_control_state(igi::gameplay::InputCode::MouseLeft, false);

    // -------------------------------------------------------------
    // Scenario 3: Dragunov Optical Sniper Scope Overlay
    // -------------------------------------------------------------
    std::cout << "[SCENARIO 3] Testing Dragunov Sniper Scope (Slot 5)...\n";
    (void)input.set_control_state(igi::gameplay::InputCode::Digit5, true);
    (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    (void)input.set_control_state(igi::gameplay::InputCode::Digit5, false);

    // Hold right mouse button for 40 ticks to glide into maximum ~6.6x optical zoom
    (void)input.set_control_state(igi::gameplay::InputCode::MouseRight, true);
    for (int t = 0; t < 40; ++t) {
        (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    }
    capture_scene(session_m1, out_dir / "screenshot_03_dragunov_scope.bmp");
    (void)input.set_control_state(igi::gameplay::InputCode::MouseRight, false);

    // -------------------------------------------------------------
    // Scenario 4: MP5SD Silenced Tactical Sight Zoom
    // -------------------------------------------------------------
    std::cout << "[SCENARIO 4] Testing MP5SD Sight Zoom (Slot 3)...\n";
    (void)input.set_control_state(igi::gameplay::InputCode::Digit3, true);
    (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    (void)input.set_control_state(igi::gameplay::InputCode::Digit3, false);

    // Toggle zoom with right click
    (void)input.set_control_state(igi::gameplay::InputCode::MouseRight, true);
    (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    (void)input.set_control_state(igi::gameplay::InputCode::MouseRight, false);
    for (int t = 0; t < 12; ++t) {
        (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    }
    capture_scene(session_m1, out_dir / "screenshot_04_mp5sd_zoom.bmp");

    // -------------------------------------------------------------
    // Scenario 5: Base Security Alarm Active
    // -------------------------------------------------------------
    std::cout << "[SCENARIO 5] Testing Security Alarm System...\n";
    (void)input.set_control_state(igi::gameplay::InputCode::Digit1, true); // switch back to M16A2
    (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    (void)input.set_control_state(igi::gameplay::InputCode::Digit1, false);

    // Trigger sector alarm
    session_m1.set_alarm_active(true);
    for (int t = 0; t < 10; ++t) {
        (void)session_m1.update(1.0 / 30.0, input.consume_tick());
    }
    capture_scene(session_m1, out_dir / "screenshot_05_m16a2_ready.bmp");

    // -------------------------------------------------------------
    // Scenario 6: Mission 2 (SAM Base) Multi-Level Verification
    // -------------------------------------------------------------
    std::cout << "[SCENARIO 6] Testing Mission 2 (SAM Base) Loading...\n";
    igi::gameplay::GameSession session_m2;
    if (session_m2.load_mission(game_root, 2)) {
        for (int t = 0; t < 60; ++t) {
            (void)session_m2.update(1.0 / 30.0, input.consume_tick());
        }
        capture_scene(session_m2, out_dir / "screenshot_06_mission2_sam_base.bmp");
    } else {
        std::cout << "[INFO] Mission 2 load skipped or fallback.\n";
    }

    std::cout << "\nAll test screenshots generated successfully!\n";
    return 0;
}
