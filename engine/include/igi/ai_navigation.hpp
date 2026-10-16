#pragma once

#include <array>

namespace igi::gameplay {

struct NavigationPosition {
    double x{};
    double y{};
    double z{};
};

struct NavigationGraphTransform {
    NavigationPosition graph_task_offset;
    std::array<std::array<float, 3>, 3> parent_orientation_basis{};
    NavigationPosition parent_world_position;
    bool parent_orientation_is_active{};
};

class AiNavigationTransform final {
public:
    // Matches the translation, optional parent basis, and world offset order
    // observed in Human/AI graph transform 0x004F9770.
    [[nodiscard]] static NavigationPosition apply_graph_transform(
        NavigationPosition graph_local_position,
        const NavigationGraphTransform& graph_transform) noexcept;
};

} // namespace igi::gameplay
