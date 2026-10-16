#include "igi/asset_io.hpp"
#include "igi/qvm_initializer.hpp"
#include "igi/qvm_task_tree.hpp"

#include "olm_parser.h"
#include "res_parser.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace
{

void require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

igi::assets::ScriptValue task_reference(std::size_t call_index)
{
    igi::assets::ScriptValue value;
    value.call_index = call_index;
    return value;
}

igi::assets::ScriptValue text_value(std::string value)
{
    igi::assets::ScriptValue result;
    result.text = std::move(value);
    return result;
}

std::vector<igi::assets::ScriptCall> make_task_tree_fixture()
{
    igi::assets::ScriptCall lightmap_info{"Task_New", std::vector<igi::assets::ScriptValue>(13)};
    lightmap_info.arguments[1].text = "LightmapInfo";
    lightmap_info.arguments[12].text = "obj00014";

    igi::assets::ScriptCall placed_object{"Task_New",
                                          {{}, text_value("EditRigidObj"), text_value("Cabinet")}};

    igi::assets::ScriptCall owner{
        "Task_New",
        {{}, text_value("Building"), text_value("SecurityBuilding"), task_reference(0), task_reference(1)}};
    return {std::move(lightmap_info), std::move(placed_object), std::move(owner)};
}

void verify_synthetic_task_tree()
{
    const auto calls = make_task_tree_fixture();
    const igi::assets::QvmTaskTree task_tree(calls);
    require(task_tree.child_call_indices(2) == std::vector<std::size_t>{0, 1},
            "QVM task children must preserve Task_New argument order");
    require(task_tree.subtree_preorder(2) == std::vector<std::size_t>{2, 0, 1},
            "QVM task traversal must preserve the owner's preorder");

    const auto& owners = task_tree.lightmap_owners();
    require(owners.size() == 1, "Synthetic LightmapInfo must produce exactly one owner");
    require(owners.front().owner_call_index == 2 && owners.front().owner_type == "Building" &&
                owners.front().owner_name == "SecurityBuilding" && owners.front().set_name == "obj00014",
            "Synthetic lightmap owner metadata must map to its containing Building task");
    require(owners.front().subtree_call_indices == std::vector<std::size_t>{2, 0, 1},
            "Synthetic lightmap owner must expose its complete ordered task subtree");

    std::vector<igi::assets::ScriptCall> shared_child_calls{{"Task_New", {text_value("EditRigidObj")}},
                                                            {"Task_New", {task_reference(0)}},
                                                            {"Task_New", {task_reference(0)}}};
    bool rejected_shared_child = false;
    try
    {
        static_cast<void>(igi::assets::QvmTaskTree(shared_child_calls));
    }
    catch (const std::exception&)
    {
        rejected_shared_child = true;
    }
    require(rejected_shared_child, "Task_New children with multiple parents must be rejected");
}

void verify_retail_level_one_lightmap_owners(const std::filesystem::path& game_directory)
{
    const auto object_script_path = game_directory / "missions/location0/level1/objects.qvm";
    const auto object_script_bytes = igi::assets::read_file(object_script_path);
    const igi::assets::QvmInitializer initializer{igi::assets::BinaryView(object_script_bytes)};
    const igi::assets::QvmTaskTree task_tree(initializer.calls());
    const auto& owners = task_tree.lightmap_owners();
    require(owners.size() == 22, "Retail Level 1 must declare 22 LightmapInfo owners");

    std::unordered_set<std::string> archived_lightmap_sets;
    std::size_t archived_lightmap_count = 0;
    std::string archive_error;
    const auto archive_path = game_directory / "missions/location0/level1/lightmaps/lightmaps.res";
    const bool archive_valid = RES_ForEachEntry(
        archive_path.string(),
        [&](const std::string& resource_name, const std::uint8_t*, std::size_t)
        {
            std::string_view set_name;
            std::uint32_t model_index = 0;
            if (igi::assets::parse_object_lightmap_entry_name(resource_name, set_name, model_index))
            {
                archived_lightmap_sets.insert(igi::assets::normalize_name(set_name));
                ++archived_lightmap_count;
            }
        },
        archive_error);
    require(archive_valid,
            archive_error.empty() ? "Retail Level 1 lightmap archive must be readable"
                                  : archive_error.c_str());
    require(archived_lightmap_count == 1308,
            "Retail Level 1 lightmap archive must expose 1,308 OLM resources");

    for (const auto& owner : owners)
    {
        require(owner.owner_type == "Building", "Retail Level 1 LightmapInfo must belong to a Building task");
        require(archived_lightmap_sets.contains(igi::assets::normalize_name(owner.set_name)),
                "Every retail LightmapInfo Filename must have a matching lightmap RES set");
        require(owner.subtree_call_indices.size() > 1,
                "Retail lightmap Building owner must expose its nested Task_New subtree");
        require(owner.subtree_call_indices.front() == owner.owner_call_index,
                "Retail lightmap subtree traversal must begin at its owning Building call");
    }
    std::cout << "[PASS] Retail Level 1 QVM lightmap ownership: " << owners.size() << " owners map to "
              << archived_lightmap_sets.size() << " resource sets across " << archived_lightmap_count
              << " OLM entries\n";
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count > 2)
        {
            throw std::runtime_error("Usage: mvp_lightmap_task_tree_probe [IGI game directory]");
        }
        verify_synthetic_task_tree();
        std::cout << "[PASS] Synthetic QVM task hierarchy and malformed parent rejection\n";
        if (argument_count == 2)
        {
            verify_retail_level_one_lightmap_owners(arguments[1]);
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr << "[FAIL] QVM lightmap task ownership: " << exception.what() << '\n';
        return 1;
    }
    return 0;
}
