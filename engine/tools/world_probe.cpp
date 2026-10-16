#include "igi/game_session.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    const std::filesystem::path root = argc > 1 ? argv[1] : "/Users/haseeb-mir/Downloads/IGI";
    const int mission_number = argc > 2 ? std::max(1, std::atoi(argv[2])) : 1;
    const std::filesystem::path out_path = argc > 3 ? argv[3] : "world_probe.ppm";

    igi::gameplay::GameSession session;
    if (!session.load_mission(root, mission_number)) {
        std::cout << "FAIL: " << session.mission_status() << '\n';
        return 1;
    }
    std::cout << "OK: " << session.mission_status() << '\n';

    igi::gameplay::InputState input;
    for (int tick = 0; tick < 120; ++tick) {
        if (!session.update(1.0 / 30.0, input.consume_tick())) {
            std::cout << "update failed at tick " << tick << '\n';
            return 1;
        }
    }
    const auto& player = session.player();
    std::cout << "after 240 ticks: z=" << player.position.z << " grounded=" << player.is_grounded
              << " vz=" << player.velocity.z << '\n';

    // Scenario: hold fire for 1 s, then walk forward for 10 s.
    const int mag_before = session.weapons().rounds_in_magazine();
    (void)input.set_control_state(igi::gameplay::InputCode::MouseLeft, true);
    float peak_recoil = 0.0f;
    for (int tick = 0; tick < 30; ++tick) {
        (void)session.update(1.0 / 30.0, input.consume_tick());
        peak_recoil = std::max(peak_recoil, session.weapons().pitch_recoil());
    }
    (void)input.set_control_state(igi::gameplay::InputCode::MouseLeft, false);
    std::cout << "fire 1s: mag " << mag_before << " -> " << session.weapons().rounds_in_magazine()
              << " peak_pitch_recoil=" << peak_recoil << '\n';

    const double start_x = player.position.x;
    const double start_y = player.position.y;
    (void)input.set_control_state(igi::gameplay::InputCode::KeyW, true);
    const auto t0 = std::chrono::steady_clock::now();
    for (int tick = 0; tick < 300; ++tick) {
        (void)session.update(1.0 / 30.0, input.consume_tick());
    }
    const auto t1 = std::chrono::steady_clock::now();
    const double moved = std::hypot(player.position.x - start_x, player.position.y - start_y);
    std::cout << "walk 10s: moved " << moved / 4096.0 << " m, health=" << player.health
              << ", ms/tick=" << std::chrono::duration<double, std::milli>(t1 - t0).count() / 300.0
              << '\n';

    std::size_t patrol = 0, combat = 0, investigate = 0, idle = 0, dead = 0;
    for (const auto& a : session.ai().agents()) {
        switch (a.state) {
            case igi::gameplay::SoldierState::Patrol: ++patrol; break;
            case igi::gameplay::SoldierState::Combat: ++combat; break;
            case igi::gameplay::SoldierState::Investigate: ++investigate; break;
            case igi::gameplay::SoldierState::Idle: ++idle; break;
            case igi::gameplay::SoldierState::Dead: ++dead; break;
        }
    }
    std::cout << "soldiers patrol=" << patrol << " combat=" << combat << " investigate=" << investigate
              << " idle=" << idle << " dead=" << dead << '\n';

    session.render(640, 400);
    const auto& px = session.renderer().pixels();
    std::ofstream out(out_path, std::ios::binary);
    out << "P6\n640 400\n255\n";
    for (std::uint32_t p : px) {
        const char rgb[3] = {static_cast<char>((p >> 16) & 0xFF), static_cast<char>((p >> 8) & 0xFF),
                             static_cast<char>(p & 0xFF)};
        out.write(rgb, 3);
    }
    std::cout << "wrote " << out_path << '\n';
    return 0;
}
