#include "igi/game_session.hpp"
#include "igi/logger.hpp"

#include <cmath>
#include <limits>

namespace igi::gameplay {

GameSession::GameSession() = default;
GameSession::~GameSession() = default;

bool GameSession::load_mission(const std::filesystem::path& game_root, int mission_number) {
    try {
        auto loaded = std::make_unique<MissionWorld>();
        if (!loaded->load_from_retail_install(game_root, mission_number)) {
            IGI_LOG_WARN("Session", "Failed to load mission " << mission_number << ": " << loaded->status_message());
            world_.reset();
            return false;
        }
        world_ = std::move(loaded);
        collision_.build_index(*world_);

        player_ = PlayerPhysicsState{};
        player_.position = world_->player_spawn_position();
        player_.yaw = world_->player_spawn_gamma();
        weapons_ = WeaponSystem{};
        ai_.initialize(*world_);
        hit_marker_seconds_ = 0.0f;
        IGI_LOG_INFO("Session", "Mission " << mission_number << " loaded and initialized successfully.");
        return true;
    } catch (const std::exception& e) {
        IGI_LOG_ERROR("Session", "Exception during load_mission: " << e.what());
        world_.reset();
        return false;
    }
}

bool GameSession::has_mission() const noexcept {
    return world_ != nullptr;
}

const std::string& GameSession::mission_status() const noexcept {
    static const std::string kNone = "no mission loaded";
    return world_ ? world_->status_message() : kNone;
}

void GameSession::add_mouse_motion(float delta_x, float delta_y) noexcept {
    if (std::isfinite(delta_x) && std::isfinite(delta_y)) {
        pending_mouse_x_ += delta_x;
        pending_mouse_y_ += delta_y;
    }
}

bool GameSession::update(
    double fixed_delta_seconds,
    const InputSnapshot& input_snapshot) noexcept {
    try {
        if (!std::isfinite(fixed_delta_seconds) || fixed_delta_seconds <= 0.0 ||
            simulation_tick_count_ == std::numeric_limits<std::uint64_t>::max()) {
            return false;
        }

    const double updated_simulation_time =
        simulation_time_seconds_ + fixed_delta_seconds;
    if (!std::isfinite(updated_simulation_time)) {
        return false;
    }

    current_input_ = input_snapshot;
    if (world_) {
        const float dt = static_cast<float>(fixed_delta_seconds);
        const std::vector<NavigationPosition> blockers = ai_.alive_positions();
        collision_.step_player(
            *world_, input_snapshot, pending_mouse_x_, pending_mouse_y_, blockers, player_);

        // Weapon slot selection (digits 1-5) and reload.
        const InputCode slot_keys[] = {
            InputCode::Digit1, InputCode::Digit2, InputCode::Digit3,
            InputCode::Digit4, InputCode::Digit5};
        for (std::size_t i = 0; i < 5; ++i) {
            if (input_snapshot.was_pressed(slot_keys[i])) {
                weapons_.select_slot(i);
            }
        }
        if (input_snapshot.was_pressed(InputCode::KeyR)) {
            weapons_.request_reload();
        }

        // Fire. The aim includes accumulated recoil, as the retail camera does.
        const WeaponFireResult fire = weapons_.update(
            dt,
            input_snapshot.is_down(InputCode::MouseLeft),
            input_snapshot.was_pressed(InputCode::MouseLeft),
            player_.eye_position(),
            player_.yaw + weapons_.yaw_recoil(),
            player_.pitch + weapons_.pitch_recoil());
        if (fire.fired) {
            ai_.notify_gunshot(fire.shot.origin, fire.audible_range_units);
            const NavigationPosition far_end{
                fire.shot.origin.x + static_cast<double>(fire.shot.direction.x) * fire.shot.range_units,
                fire.shot.origin.y + static_cast<double>(fire.shot.direction.y) * fire.shot.range_units,
                fire.shot.origin.z + static_cast<double>(fire.shot.direction.z) * fire.shot.range_units,
            };
            const RaycastHit wall = collision_.raycast(*world_, fire.shot.origin, far_end, false, true);
            const float limit = wall.hit ? wall.fraction : 1.0f;
            if (ai_.apply_shot(fire.shot, limit).hit) {
                hit_marker_seconds_ = 0.2f;
            }
        }
        hit_marker_seconds_ = std::max(0.0f, hit_marker_seconds_ - dt);

        const float damage = ai_.update(dt, *world_, collision_, player_);
        player_.health = std::max(0.0f, player_.health - damage);
    }
    pending_mouse_x_ = 0.0f;
    pending_mouse_y_ = 0.0f;

    simulation_time_seconds_ = updated_simulation_time;
    ++simulation_tick_count_;
    return true;
    } catch (const std::exception& e) {
        IGI_LOG_ERROR("Session", "Exception in GameSession::update: " << e.what());
        return false;
    } catch (...) {
        IGI_LOG_FATAL("Session", "Unknown fatal exception in GameSession::update");
        return false;
    }
}

void GameSession::render(std::uint32_t width, std::uint32_t height) {
    if (width != renderer_.width() || height != renderer_.height()) {
        renderer_.resize(width, height);
    }
    if (!world_) {
        return;
    }
    CameraView camera;
    camera.eye = player_.eye_position();
    camera.yaw = player_.yaw + weapons_.yaw_recoil();
    camera.pitch = player_.pitch + weapons_.pitch_recoil();
    renderer_.render_world(*world_, camera, ai_.alive_positions());
    HudState hud;
    hud.health_fraction = player_.health / 100.0f;
    const int magazine = weapons_.current_stats().magazine_capacity;
    hud.ammo_fraction = magazine > 0
        ? static_cast<float>(weapons_.rounds_in_magazine()) / static_cast<float>(magazine)
        : 0.0f;
    hud.hit_marker_active = hit_marker_seconds_ > 0.0f;
    hud.muzzle_flash = weapons_.muzzle_flash();
    renderer_.render_hud(hud);
}

const SoftwareRenderer& GameSession::renderer() const noexcept {
    return renderer_;
}

const PlayerPhysicsState& GameSession::player() const noexcept {
    return player_;
}

std::uint64_t GameSession::simulation_tick_count() const noexcept {
    return simulation_tick_count_;
}

double GameSession::simulation_time_seconds() const noexcept {
    return simulation_time_seconds_;
}

const InputSnapshot& GameSession::current_input() const noexcept {
    return current_input_;
}

void GameSession::reset() noexcept {
    simulation_tick_count_ = 0;
    simulation_time_seconds_ = 0.0;
    current_input_ = {};
    pending_mouse_x_ = 0.0f;
    pending_mouse_y_ = 0.0f;
    if (world_) {
        player_ = PlayerPhysicsState{};
        player_.position = world_->player_spawn_position();
        player_.yaw = world_->player_spawn_gamma();
    }
}

} // namespace igi::gameplay
