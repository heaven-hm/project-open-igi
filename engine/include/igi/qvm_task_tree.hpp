#pragma once

#include "igi/qvm_initializer.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace igi::assets
{

struct QvmLightmapOwner final
{
    std::size_t owner_call_index{};
    std::string owner_type;
    std::string owner_name;
    std::string set_name;
    std::vector<std::size_t> subtree_call_indices;
};

class QvmTaskTree final
{
  public:
    explicit QvmTaskTree(const std::vector<ScriptCall>& calls);

    [[nodiscard]] const std::vector<std::size_t>& child_call_indices(std::size_t task_call_index) const;
    [[nodiscard]] std::vector<std::size_t> subtree_preorder(std::size_t task_call_index) const;

    [[nodiscard]] const std::vector<QvmLightmapOwner>& lightmap_owners() const noexcept
    {
        return lightmap_owners_;
    }

  private:
    std::vector<std::vector<std::size_t>> child_call_indices_;
    std::vector<bool> is_task_call_;
    std::vector<QvmLightmapOwner> lightmap_owners_;
};

} // namespace igi::assets
