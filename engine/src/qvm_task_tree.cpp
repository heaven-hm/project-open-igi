#include "igi/qvm_task_tree.hpp"

#include <limits>
#include <stdexcept>

namespace igi::assets
{
namespace
{

constexpr std::string_view task_new_function = "Task_New";
constexpr std::string_view lightmap_info_type = "LightmapInfo";
constexpr std::size_t lightmap_filename_argument_index = 12;

} // namespace

QvmTaskTree::QvmTaskTree(const std::vector<ScriptCall>& calls)
    : child_call_indices_(calls.size()), is_task_call_(calls.size(), false)
{
    const std::size_t no_parent = std::numeric_limits<std::size_t>::max();
    std::vector<std::size_t> parent_call_indices(calls.size(), no_parent);
    for (std::size_t call_index = 0; call_index < calls.size(); ++call_index)
    {
        is_task_call_[call_index] = calls[call_index].function == task_new_function;
    }

    for (std::size_t parent_call_index = 0; parent_call_index < calls.size(); ++parent_call_index)
    {
        if (!is_task_call_[parent_call_index])
        {
            continue;
        }

        for (const ScriptValue& argument : calls[parent_call_index].arguments)
        {
            const std::size_t child_call_index = argument.call_index;
            if (child_call_index == no_parent)
            {
                continue;
            }
            if (child_call_index >= calls.size())
            {
                throw std::runtime_error("QVM task argument references a call outside the initializer list");
            }
            if (!is_task_call_[child_call_index])
            {
                continue;
            }
            if (child_call_index >= parent_call_index)
            {
                throw std::runtime_error("QVM Task_New child does not precede its parent call");
            }
            if (parent_call_indices[child_call_index] != no_parent)
            {
                throw std::runtime_error("QVM Task_New call is referenced by more than one parent");
            }

            parent_call_indices[child_call_index] = parent_call_index;
            child_call_indices_[parent_call_index].push_back(child_call_index);
        }
    }

    for (std::size_t owner_call_index = 0; owner_call_index < calls.size(); ++owner_call_index)
    {
        if (!is_task_call_[owner_call_index])
        {
            continue;
        }

        std::size_t lightmap_info_call_index = no_parent;
        for (const std::size_t child_call_index : child_call_indices_[owner_call_index])
        {
            const ScriptCall& child_call = calls[child_call_index];
            if (child_call.arguments.size() > 1 && child_call.arguments[1].text == lightmap_info_type)
            {
                lightmap_info_call_index = child_call_index;
                break;
            }
        }
        if (lightmap_info_call_index == no_parent)
        {
            continue;
        }

        const ScriptCall& owner_call = calls[owner_call_index];
        const ScriptCall& lightmap_info_call = calls[lightmap_info_call_index];
        if (lightmap_info_call.arguments.size() <= lightmap_filename_argument_index)
        {
            throw std::runtime_error("LightmapInfo Task_New call is missing its Filename argument");
        }
        const std::string& set_name = lightmap_info_call.arguments[lightmap_filename_argument_index].text;
        if (set_name.empty())
        {
            continue;
        }

        QvmLightmapOwner owner;
        owner.owner_call_index = owner_call_index;
        owner.owner_type = owner_call.arguments.size() > 1 ? owner_call.arguments[1].text : std::string{};
        owner.owner_name = owner_call.arguments.size() > 2 ? owner_call.arguments[2].text : std::string{};
        owner.set_name = set_name;
        owner.subtree_call_indices = subtree_preorder(owner_call_index);
        lightmap_owners_.push_back(std::move(owner));
    }
}

const std::vector<std::size_t>& QvmTaskTree::child_call_indices(std::size_t task_call_index) const
{
    if (task_call_index >= is_task_call_.size() || !is_task_call_[task_call_index])
    {
        throw std::out_of_range("QVM task tree lookup requires a valid Task_New call index");
    }
    return child_call_indices_[task_call_index];
}

std::vector<std::size_t> QvmTaskTree::subtree_preorder(std::size_t task_call_index) const
{
    if (task_call_index >= is_task_call_.size() || !is_task_call_[task_call_index])
    {
        throw std::out_of_range("QVM task subtree requires a valid Task_New call index");
    }

    std::vector<std::size_t> subtree_call_indices;
    std::vector<std::size_t> pending_call_indices{task_call_index};
    while (!pending_call_indices.empty())
    {
        const std::size_t current_call_index = pending_call_indices.back();
        pending_call_indices.pop_back();
        subtree_call_indices.push_back(current_call_index);

        const auto& children = child_call_indices_[current_call_index];
        pending_call_indices.insert(pending_call_indices.end(), children.rbegin(), children.rend());
    }
    return subtree_call_indices;
}

} // namespace igi::assets
