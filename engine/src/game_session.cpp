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
        last_terrain_center_x_ = player_.position.x;
        last_terrain_center_y_ = player_.position.y;
        weapons_ = WeaponSystem{};
        sight_zoom_.set_weapon(weapons_.current_stats().name);
        view_bob_.reset();
        ai_.initialize(*world_);
        level_flow_.initialize_mission(static_cast<std::uint32_t>(mission_number));
        expression_state_.clear();
        hit_marker_seconds_ = 0.0f;
        hit_marker_headshot_ = false;
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
        const float mag = sight_zoom_.magnification();
        const float scale = (mag > 1.0f) ? (1.0f / mag) : 1.0f;
        pending_mouse_x_ += delta_x * scale;
        pending_mouse_y_ += delta_y * scale;
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

        // Dynamic continuous terrain streaming around player
        const double dtx = player_.position.x - last_terrain_center_x_;
        const double dty = player_.position.y - last_terrain_center_y_;
        if (dtx * dtx + dty * dty > (16.0 * 4096.0) * (16.0 * 4096.0)) {
            world_->extract_nearby_terrain_mesh(player_.position.x, player_.position.y, 160.0 * 4096.0);
            last_terrain_center_x_ = player_.position.x;
            last_terrain_center_y_ = player_.position.y;
        }

        // Weapon slot selection (digits 1-5, mouse wheel, or Q to cycle)
        const InputCode slot_keys[] = {
            InputCode::Digit1, InputCode::Digit2, InputCode::Digit3,
            InputCode::Digit4, InputCode::Digit5};
        for (std::size_t i = 0; i < 5; ++i) {
            if (input_snapshot.was_pressed(slot_keys[i]) || input_snapshot.is_down(slot_keys[i])) {
                weapons_.select_slot(i);
                sight_zoom_.set_weapon(weapons_.current_stats().name);
            }
        }
        if (input_snapshot.was_pressed(InputCode::MouseWheelDown) ||
            input_snapshot.was_pressed(InputCode::KeyQ)) {
            weapons_.cycle_next();
            sight_zoom_.set_weapon(weapons_.current_stats().name);
        } else if (input_snapshot.was_pressed(InputCode::MouseWheelUp)) {
            weapons_.cycle_prev();
            sight_zoom_.set_weapon(weapons_.current_stats().name);
        }
        if (input_snapshot.was_pressed(InputCode::KeyR)) {
            weapons_.request_reload();
        }

        // Sight zoom tick (alternate fire / right mouse click)
        sight_zoom_.tick(
            input_snapshot.was_pressed(InputCode::MouseRight),
            input_snapshot.is_down(InputCode::MouseRight));

        // Player view bobbing update
        const float horiz_speed = std::hypot(player_.velocity.x, player_.velocity.y);
        view_bob_.update(dt, player_.is_grounded, horiz_speed, player_.is_running, player_.is_crouching);

        // Player interaction with world objects (doors, terminals, pickups)
        if (input_snapshot.was_pressed(InputCode::KeyF) || input_snapshot.was_pressed(InputCode::KeyE)) {
            const auto interact = world_->try_interact(player_.position, player_.yaw);
            if (interact.interacted) {
                if (!interact.script_variable.empty()) {
                    expression_state_.set_boolean(interact.script_variable, true);
                    expression_state_.set_number(interact.script_variable, 1.0);
                }
                if (interact.ammo_count > 0) {
                    weapons_.add_reserve_ammo(interact.ammo_count);
                }
            }
        }

        // Advance dynamic world entities (doors)
        world_->update_doors(dt);

        // Fire. The aim includes accumulated recoil, as the retail camera does.
        const bool fire_held = input_snapshot.is_down(InputCode::MouseLeft) ||
                               input_snapshot.is_down(InputCode::LeftControl);
        const bool fire_pressed = input_snapshot.was_pressed(InputCode::MouseLeft) ||
                                  input_snapshot.was_pressed(InputCode::LeftControl);
        const WeaponFireResult fire = weapons_.update(
            dt,
            fire_held,
            fire_pressed,
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
            const SoldierShotHit shot_hit = ai_.apply_shot(fire.shot, limit);
            if (shot_hit.hit) {
                hit_marker_seconds_ = 0.25f;
                hit_marker_headshot_ = shot_hit.is_headshot;
            } else if (wall.hit && wall.object_index >= 0) {
                if (world_->damage_object(wall.object_index, fire.shot.damage)) {
                    hit_marker_seconds_ = 0.25f;
                    hit_marker_headshot_ = false;
                }
            }
        }
        hit_marker_seconds_ = std::max(0.0f, hit_marker_seconds_ - dt);

        if (player_.last_fall_hearing_alert > 0.0f) {
            ai_.notify_gunshot(player_.position, player_.last_fall_hearing_alert);
            player_.last_fall_hearing_alert = 0.0f;
        }

        const float damage = ai_.update(dt, *world_, collision_, player_);
        player_.health = std::max(0.0f, player_.health - damage);

        level_flow_.update(
            player_.health > 0.0f,
            false,
            [this](const std::string& expression) -> bool {
                bool result = false;
                return expression_state_.try_evaluate_boolean(expression, result) && result;
            });
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

    // Apply view bobbing
    const ViewBobOffset bob = view_bob_.current_offset();
    camera.eye.z += static_cast<double>(bob.vertical_offset);
    const float cy = std::cos(player_.yaw);
    const float sy = std::sin(player_.yaw);
    camera.eye.x += static_cast<double>(sy * bob.lateral_offset);
    camera.eye.y -= static_cast<double>(cy * bob.lateral_offset);

    camera.yaw = player_.yaw + weapons_.yaw_recoil() + weapons_.view_sway_yaw() + weapons_.view_recoil_yaw();
    camera.pitch = player_.pitch + weapons_.pitch_recoil() + weapons_.view_sway_pitch() + weapons_.view_recoil_pitch() + bob.pitch_bob;
    camera.vertical_fov_radians = sight_zoom_.current_fov_radians();

    std::vector<SoldierRenderData> soldier_renders;
    soldier_renders.reserve(ai_.agents().size());
    for (const auto& a : ai_.agents()) {
        SoldierRenderData srd;
        srd.position = a.position;
        srd.yaw = a.yaw;
        srd.is_alive = a.alive();
        srd.is_combat = (a.state == SoldierState::Combat);
        srd.walk_phase = a.walk_phase;
        srd.is_firing = a.is_firing;
        soldier_renders.push_back(srd);
    }
    renderer_.render_world(*world_, camera, soldier_renders);

    HudState hud;
    hud.health_fraction = player_.health / 100.0f;
    hud.health_percent = std::clamp(static_cast<int>(player_.health), 0, 100);
    const auto& cur_wep = weapons_.current_stats();
    const int magazine = cur_wep.magazine_capacity;
    hud.ammo_fraction = magazine > 0
        ? static_cast<float>(weapons_.rounds_in_magazine()) / static_cast<float>(magazine)
        : 0.0f;
    hud.rounds_in_magazine = weapons_.rounds_in_magazine();
    hud.reserve_ammo = weapons_.reserve_ammo();
    hud.weapon_name = cur_wep.name;
    hud.fire_mode = (cur_wep.burst_count == 1) ? "SEMI" : ((cur_wep.burst_count >= magazine) ? "AUTO" : "BURST");
    hud.hit_marker_active = hit_marker_seconds_ > 0.0f;
    hud.is_headshot = hit_marker_headshot_;
    hud.muzzle_flash = weapons_.muzzle_flash();
    hud.objective_text = level_flow_.get_objective_display_text();
    hud.interaction_prompt = world_->get_interaction_hint(player_.eye_position(), player_.yaw);
    hud.mission_success = (level_flow_.status() == MissionStatus::Success);
    hud.mission_failed = (level_flow_.status() == MissionStatus::Failed);
    hud.is_alarm_active = world_->is_alarm_active();
    hud.is_sniper_scope = (sight_zoom_.mode() == ZoomMode::DragunovScope && sight_zoom_.is_active());
    hud.zoom_magnification = sight_zoom_.magnification();

    renderer_.render_first_person_weapon(
        hud,
        view_bob_.phase(),
        weapons_.view_sway_yaw(),
        weapons_.view_sway_pitch());
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
    sight_zoom_.reset();
    view_bob_.reset();
    hit_marker_seconds_ = 0.0f;
    hit_marker_headshot_ = false;
    if (world_) {
        player_ = PlayerPhysicsState{};
        player_.position = world_->player_spawn_position();
        player_.yaw = world_->player_spawn_gamma();
    }
}

} // namespace igi::gameplay
