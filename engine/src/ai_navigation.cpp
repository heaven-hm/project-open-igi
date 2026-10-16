#include "igi/ai_navigation.hpp"

namespace igi::gameplay {

NavigationPosition AiNavigationTransform::apply_graph_transform(
    NavigationPosition graph_local_position,
    const NavigationGraphTransform& graph_transform) noexcept {
    const NavigationPosition task_translated_position{
        graph_local_position.x + graph_transform.graph_task_offset.x,
        graph_local_position.y + graph_transform.graph_task_offset.y,
        graph_local_position.z + graph_transform.graph_task_offset.z,
    };

    if (!graph_transform.parent_orientation_is_active) {
        return task_translated_position;
    }

    const auto& basis_rows = graph_transform.parent_orientation_basis;
    const NavigationPosition parent_local_position{
        static_cast<double>(basis_rows[0][0]) * task_translated_position.x +
            static_cast<double>(basis_rows[0][1]) * task_translated_position.y +
            static_cast<double>(basis_rows[0][2]) * task_translated_position.z,
        static_cast<double>(basis_rows[1][0]) * task_translated_position.x +
            static_cast<double>(basis_rows[1][1]) * task_translated_position.y +
            static_cast<double>(basis_rows[1][2]) * task_translated_position.z,
        static_cast<double>(basis_rows[2][0]) * task_translated_position.x +
            static_cast<double>(basis_rows[2][1]) * task_translated_position.y +
            static_cast<double>(basis_rows[2][2]) * task_translated_position.z,
    };

    return {
        parent_local_position.x + graph_transform.parent_world_position.x,
        parent_local_position.y + graph_transform.parent_world_position.y,
        parent_local_position.z + graph_transform.parent_world_position.z,
    };
}

} // namespace igi::gameplay
