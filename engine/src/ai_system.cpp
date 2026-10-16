#include "igi/ai_system.hpp"
#include "igi/logger.hpp"

#include <algorithm>
#include <cmath>

namespace igi::gameplay {

namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kRetailTickSeconds = 1.0f / 30.0f;
constexpr float kWalkSpeedUnitsPerSecond = 3.0f * 4096.0f;   // provisional 3 m/s
constexpr float kRunSpeedUnitsPerSecond = 6.0f * 4096.0f;    // provisional 6 m/s
constexpr float kFireIntervalSeconds = 0.18f;                // provisional
constexpr float kSoldierShotDamage = 7.0f;                   // provisional
constexpr float kLoseSightSeconds = 4.0f;
constexpr float kPerceptionIntervalSeconds = 4.0f * kRetailTickSeconds;
constexpr float kEyeHeight = 6553.6f;                        // 1.6 m
constexpr float kPrimaryHalfFovCos = 0.5f;                   // 60 degrees half angle (provisional)
constexpr float kSecondaryHalfFovCos = -0.2f;                // wide close-range awareness

[[nodiscard]] float wrap_pi(float a) noexcept {
    while (a > kPi) a -= 2.0f * kPi;
    while (a < -kPi) a += 2.0f * kPi;
    return a;
}

} // namespace

float AiSystem::next_random() noexcept {
    rng_state_ ^= rng_state_ << 13;
    rng_state_ ^= rng_state_ >> 17;
    rng_state_ ^= rng_state_ << 5;
    return static_cast<float>(rng_state_ & 0xFFFFFFu) / 16777216.0f;
}

void AiSystem::initialize(const MissionWorld& world) {
    try {
        agents_.clear();
        const auto& spawns = world.soldier_spawns();
        agents_.reserve(spawns.size());
        for (std::size_t i = 0; i < spawns.size(); ++i) {
            SoldierAgent agent;
            agent.spawn_index = i;
            agent.position = spawns[i].position;
            agent.yaw = spawns[i].gamma;
            agent.state = spawns[i].patrol_commands.empty() ? SoldierState::Idle : SoldierState::Patrol;
            agent.perception_timer = next_random() * kPerceptionIntervalSeconds;
            agents_.push_back(agent);
        }
        IGI_LOG_INFO("AI", "Initialized " << agents_.size() << " soldier agents.");
    } catch (const std::exception& e) {
        IGI_LOG_ERROR("AI", "Exception during AiSystem::initialize: " << e.what());
    }
}

std::size_t AiSystem::alive_count() const noexcept {
    return static_cast<std::size_t>(std::count_if(
        agents_.begin(), agents_.end(), [](const SoldierAgent& a) { return a.alive(); }));
}

std::vector<NavigationPosition> AiSystem::alive_positions() const {
    std::vector<NavigationPosition> positions;
    for (const SoldierAgent& agent : agents_) {
        if (agent.alive()) {
            positions.push_back(agent.position);
        }
    }
    return positions;
}

void AiSystem::turn_toward(SoldierAgent& agent, NavigationPosition target, float dt) noexcept {
    const float desired = std::atan2(
        static_cast<float>(target.y - agent.position.y),
        static_cast<float>(target.x - agent.position.x));
    const float diff = wrap_pi(desired - agent.yaw);
    const float max_step = kTurnRadiansPerRetailTick * (dt / kRetailTickSeconds);
    agent.yaw = wrap_pi(agent.yaw + std::clamp(diff, -max_step, max_step));
}

bool AiSystem::step_toward(
    SoldierAgent& agent,
    const WorldCollision& collision,
    const MissionWorld& world,
    NavigationPosition target,
    float speed_units_per_second,
    float dt) noexcept {
    const double dx = target.x - agent.position.x;
    const double dy = target.y - agent.position.y;
    const double dist = std::sqrt(dx * dx + dy * dy);
    if (dist <= static_cast<double>(kWaypointArrivalUnits)) {
        return true;
    }
    turn_toward(agent, target, dt);

    // Only advance once roughly facing the target.
    const float desired = std::atan2(static_cast<float>(dy), static_cast<float>(dx));
    if (std::fabs(wrap_pi(desired - agent.yaw)) > 0.6f) {
        return false;
    }
    const double step = std::min(dist, static_cast<double>(speed_units_per_second * dt));
    const NavigationPosition desired_pos{
        agent.position.x + dx / dist * step,
        agent.position.y + dy / dist * step,
        agent.position.z,
    };
    const WallSweepResult sweep = collision.sweep_walls(
        world, agent.position, desired_pos, {}, true, false);
    NavigationPosition next = sweep.resolved_position;
    const GroundQueryResult ground = collision.query_ground(world, next, true, false, true);
    if (ground.found_ground) {
        next.z = ground.ground_z;
    }
    agent.position = next;
    return false;
}

void AiSystem::update_perception(
    SoldierAgent& agent,
    const MissionWorld& world,
    const WorldCollision& collision,
    const PlayerPhysicsState& player) noexcept {
    const double dx = player.position.x - agent.position.x;
    const double dy = player.position.y - agent.position.y;
    const double dist = std::sqrt(dx * dx + dy * dy);
    const double primary = static_cast<double>(world.is_alarm_active() ? 60.0f : kPrimarySightMeters) * 4096.0;
    const double secondary = static_cast<double>(world.is_alarm_active() ? 25.0f : kSecondarySightMeters) * 4096.0;

    bool in_cone = false;
    if (dist > 1.0) {
        const float facing_dot = static_cast<float>(
            (dx * std::cos(agent.yaw) + dy * std::sin(agent.yaw)) / dist);
        in_cone = (dist <= primary && facing_dot >= kPrimaryHalfFovCos) ||
                  (dist <= secondary && facing_dot >= kSecondaryHalfFovCos);
    }

    bool visible = false;
    if (in_cone) {
        const NavigationPosition from{agent.position.x, agent.position.y, agent.position.z + kEyeHeight};
        const NavigationPosition to = player.eye_position();
        const RaycastHit hit = collision.raycast(world, from, to, false, true);
        visible = !hit.hit || hit.fraction >= kLosClearFraction;
    }

    agent.sees_player = visible;
    if (visible) {
        agent.last_known_player = player.position;
        agent.lose_sight_timer = 0.0f;
        agent.state = SoldierState::Combat;
    }
}

void AiSystem::update_patrol(SoldierAgent& agent, const MissionWorld& world, float dt) noexcept {
    const SoldierSpawn& spawn = world.soldier_spawns()[agent.spawn_index];
    if (agent.wait_seconds > 0.0f) {
        agent.wait_seconds = std::max(0.0f, agent.wait_seconds - dt);
        return;
    }
    if (agent.has_walk_target) {
        return; // movement handled by caller
    }
    if (spawn.patrol_commands.empty()) {
        agent.state = SoldierState::Idle;
        return;
    }
    if (agent.patrol_cursor >= spawn.patrol_commands.size()) {
        agent.patrol_cursor = 0;
    }

    const PatrolCommand& cmd = spawn.patrol_commands[agent.patrol_cursor++];
    const auto graph_it = world.world_navigation_graphs().find(spawn.graph_id);
    const auto find_node = [&](int id, NavigationPosition& out) {
        if (graph_it == world.world_navigation_graphs().end()) {
            return false;
        }
        for (const auto& node : graph_it->second.nodes) {
            if (node.id == id) {
                out = {node.x, node.y, node.z};
                return true;
            }
        }
        return false;
    };

    switch (cmd.opcode) {
        case 1: // delay (argument in retail ticks)
            agent.wait_seconds = static_cast<float>(cmd.argument) * kRetailTickSeconds;
            break;
        case 2:
        case 3: { // walk / run to node
            NavigationPosition target{};
            if (find_node(cmd.argument, target)) {
                agent.walk_target = target;
                agent.has_walk_target = true;
                agent.walk_is_run = cmd.opcode == 3;
            }
            break;
        }
        case 5: { // look at node
            NavigationPosition target{};
            if (find_node(cmd.argument, target)) {
                agent.yaw = std::atan2(
                    static_cast<float>(target.y - agent.position.y),
                    static_cast<float>(target.x - agent.position.x));
            }
            break;
        }
        case 6:
        case 7: // end / quit patrol (loop behaviour not yet verified)
            agent.state = SoldierState::Idle;
            break;
        default:
            break;
    }
}

float AiSystem::update(
    float delta_seconds,
    const MissionWorld& world,
    const WorldCollision& collision,
    const PlayerPhysicsState& player) noexcept {
    float player_damage = 0.0f;
    const double player_dist_limit = 120.0 * 4096.0;

    for (SoldierAgent& agent : agents_) {
        if (!agent.alive()) {
            agent.death_seconds += delta_seconds;
            continue;
        }
        // Skip simulation for far-away idle soldiers to bound cost.
        const double fx = agent.position.x - player.position.x;
        const double fy = agent.position.y - player.position.y;
        if (fx * fx + fy * fy > player_dist_limit * player_dist_limit &&
            agent.state != SoldierState::Combat) {
            continue;
        }

        agent.perception_timer -= delta_seconds;
        if (agent.perception_timer <= 0.0f) {
            agent.perception_timer += kPerceptionIntervalSeconds;
            update_perception(agent, world, collision, player);
        }

        switch (agent.state) {
            case SoldierState::Patrol: {
                update_patrol(agent, world, delta_seconds);
                if (agent.has_walk_target) {
                    const bool arrived = step_toward(
                        agent, collision, world, agent.walk_target,
                        agent.walk_is_run ? kRunSpeedUnitsPerSecond : kWalkSpeedUnitsPerSecond,
                        delta_seconds);
                    if (arrived) {
                        agent.has_walk_target = false;
                    }
                }
                break;
            }
            case SoldierState::Investigate: {
                const bool arrived = step_toward(
                    agent, collision, world, agent.last_known_player,
                    kRunSpeedUnitsPerSecond, delta_seconds);
                if (arrived) {
                    agent.state = SoldierState::Idle;
                }
                break;
            }
            case SoldierState::Combat: {
                if (!agent.sees_player) {
                    agent.lose_sight_timer += delta_seconds;
                    if (agent.lose_sight_timer >= kLoseSightSeconds) {
                        agent.state = SoldierState::Investigate;
                    } else {
                        (void)step_toward(
                            agent, collision, world, agent.last_known_player,
                            kRunSpeedUnitsPerSecond, delta_seconds);
                    }
                    break;
                }
                turn_toward(agent, player.position, delta_seconds);
                const double dx = player.position.x - agent.position.x;
                const double dy = player.position.y - agent.position.y;
                const double dist = std::sqrt(dx * dx + dy * dy);
                // Close in until within 12 m, then hold position and shoot.
                if (dist > 12.0 * 4096.0) {
                    (void)step_toward(
                        agent, collision, world, player.position,
                        kRunSpeedUnitsPerSecond, delta_seconds);
                }
                agent.fire_timer -= delta_seconds;
                if (agent.fire_timer <= 0.0f && dist > 1.0) {
                    const float facing = static_cast<float>(
                        (dx * std::cos(agent.yaw) + dy * std::sin(agent.yaw)) / dist);
                    if (facing >= kFireConeCosine) {
                        agent.fire_timer = kFireIntervalSeconds;
                        // Accuracy falls off with distance (provisional).
                        const float hit_chance = std::clamp(
                            0.55f - static_cast<float>(dist) / (60.0f * 4096.0f), 0.08f, 0.55f);
                        if (next_random() < hit_chance) {
                            player_damage += kSoldierShotDamage;
                        }
                    }
                }
                break;
            }
            case SoldierState::Idle:
            case SoldierState::Dead:
                break;
        }
    }
    return player_damage;
}

void AiSystem::notify_gunshot(NavigationPosition origin, float audible_range_units) noexcept {
    const double range_sq = static_cast<double>(audible_range_units) * audible_range_units;
    for (SoldierAgent& agent : agents_) {
        if (!agent.alive() || agent.state == SoldierState::Combat) {
            continue;
        }
        const double dx = agent.position.x - origin.x;
        const double dy = agent.position.y - origin.y;
        const double dz = agent.position.z - origin.z;
        if (dx * dx + dy * dy + dz * dz <= range_sq) {
            agent.last_known_player = origin;
            agent.has_walk_target = false;
            agent.state = SoldierState::Investigate;
            IGI_LOG_DEBUG("AI", "Soldier " << agent.spawn_index << " alerted by gunshot.");
        }
    }
}

SoldierShotHit AiSystem::apply_shot(const ShotRay& shot, float max_fraction) noexcept {
    SoldierShotHit best;
    best.fraction = max_fraction;
    for (std::size_t i = 0; i < agents_.size(); ++i) {
        SoldierAgent& agent = agents_[i];
        if (!agent.alive()) {
            continue;
        }
        // Ray vs vertical cylinder: solve in XY, then check Z span.
        const double ox = shot.origin.x - agent.position.x;
        const double oy = shot.origin.y - agent.position.y;
        const double dx = shot.direction.x;
        const double dy = shot.direction.y;
        const double a = dx * dx + dy * dy;
        if (a < 1.0e-9) {
            continue;
        }
        const double b = 2.0 * (ox * dx + oy * dy);
        const double c = ox * ox + oy * oy - static_cast<double>(kCylinderRadius) * kCylinderRadius;
        const double disc = b * b - 4.0 * a * c;
        if (disc < 0.0) {
            continue;
        }
        const double t = (-b - std::sqrt(disc)) / (2.0 * a); // distance in units (|dir|=1)
        if (t < 0.0 || t > static_cast<double>(shot.range_units)) {
            continue;
        }
        const double hit_z = shot.origin.z + shot.direction.z * t;
        if (hit_z < agent.position.z || hit_z > agent.position.z + kCylinderHeight) {
            continue;
        }
        const double rel_z = hit_z - agent.position.z;
        const float fraction = static_cast<float>(t / static_cast<double>(shot.range_units));
        if (fraction < best.fraction) {
            best.hit = true;
            best.soldier_index = i;
            best.fraction = fraction;
            best.is_headshot = (rel_z >= 5800.0);
        }
    }
    if (best.hit) {
        SoldierAgent& victim = agents_[best.soldier_index];
        const float damage_mult = best.is_headshot ? 2.5f : 1.0f;
        victim.health -= shot.damage * damage_mult;
        victim.last_known_player = shot.origin;
        victim.lose_sight_timer = 0.0f;
        if (victim.health <= 0.0f) {
            victim.state = SoldierState::Dead;
            best.is_lethal = true;
            IGI_LOG_INFO("AI", "Soldier " << victim.spawn_index << " killed ("
                         << (best.is_headshot ? "HEADSHOT" : "body") << ").");
        } else {
            victim.state = SoldierState::Investigate;
            best.is_lethal = false;
            IGI_LOG_DEBUG("AI", "Soldier " << victim.spawn_index << " wounded (health=" << victim.health << ").");
        }
    }
    return best;
}

} // namespace igi::gameplay
