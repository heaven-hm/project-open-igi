#include "igi/game_material_registry.hpp"

#include "igi/asset_io.hpp"
#include "igi/exceptions.hpp"
#include "igi/qvm_initializer.hpp"

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <utility>

namespace igi::assets
{
namespace
{

constexpr std::string_view set_active_material_native = "SetActiveMaterial";
constexpr std::string_view define_q_material_native = "DefineQMaterial";
constexpr std::string_view define_game_material_native = "DefineGameMaterial";

std::size_t GameMaterialRegistry_ReadMaterialIndex(const ScriptCall& script_call)
{
    if (script_call.arguments.size() != 1)
    {
        throw logging::ScriptException("SetActiveMaterial must contain exactly one argument");
    }

    const ScriptValue& source_value = script_call.arguments.front();
    if (source_value.call_index != std::numeric_limits<std::size_t>::max() ||
        !source_value.text.empty() || !std::isfinite(source_value.number) ||
        std::trunc(source_value.number) != source_value.number || source_value.number < 0.0 ||
        source_value.number > static_cast<double>(std::numeric_limits<std::uint16_t>::max()))
    {
        throw logging::ScriptException("SetActiveMaterial contains an invalid material index");
    }

    return static_cast<std::size_t>(source_value.number);
}

} // namespace

std::size_t GameMaterialRegistry::GameMaterialRegistry_LoadForGame(
    const std::filesystem::path& game_directory)
{
    if (game_directory.empty())
    {
        throw logging::ScriptException("Game material load received an empty game directory");
    }

    const std::filesystem::path script_path = resolve_path(game_directory, "material/material.qvm");
    return GameMaterialRegistry_LoadScript(script_path);
}

std::size_t GameMaterialRegistry::GameMaterialRegistry_LoadScript(
    const std::filesystem::path& script_path)
{
    const FileBytes script_bytes = read_file(script_path);
    const QvmInitializer initializer{BinaryView(script_bytes)};
    return GameMaterialRegistry_ParseCalls(initializer.calls());
}

std::size_t GameMaterialRegistry::GameMaterialRegistry_ParseCalls(
    std::span<const ScriptCall> script_calls)
{
    std::vector<GameMaterialSourceDefinition> parsed_definitions;
    std::unordered_map<std::size_t, std::size_t> parsed_definition_by_material_index;
    std::size_t active_material_index = std::numeric_limits<std::size_t>::max();
    for (const ScriptCall& script_call : script_calls)
    {
        if (script_call.function == set_active_material_native)
        {
            if (active_material_index != std::numeric_limits<std::size_t>::max())
            {
                throw logging::ScriptException(
                    "SetActiveMaterial was not followed by DefineGameMaterial");
            }
            active_material_index = GameMaterialRegistry_ReadMaterialIndex(script_call);
            continue;
        }

        if (script_call.function == define_q_material_native)
        {
            if (active_material_index == std::numeric_limits<std::size_t>::max())
            {
                throw logging::ScriptException("DefineQMaterial has no active material row");
            }
            continue;
        }

        if (script_call.function != define_game_material_native)
        {
            throw logging::ScriptException(
                "Material QVM contains an unsupported native: " + script_call.function);
        }
        if (active_material_index == std::numeric_limits<std::size_t>::max())
        {
            throw logging::ScriptException("DefineGameMaterial has no active material row");
        }
        if (script_call.arguments.empty() || script_call.arguments.front().text.empty() ||
            script_call.arguments.front().call_index != std::numeric_limits<std::size_t>::max())
        {
            throw logging::ScriptException("DefineGameMaterial has no source material name");
        }

        GameMaterialSourceDefinition definition;
        definition.material_index = active_material_index;
        definition.source_arguments = script_call.arguments;
        const std::size_t definition_index = parsed_definitions.size();
        if (!parsed_definition_by_material_index
                 .emplace(active_material_index, definition_index)
                 .second)
        {
            throw logging::ScriptException(
                "Material QVM repeats material row " + std::to_string(active_material_index));
        }
        parsed_definitions.push_back(std::move(definition));
        active_material_index = std::numeric_limits<std::size_t>::max();
    }

    if (active_material_index != std::numeric_limits<std::size_t>::max())
    {
        throw logging::ScriptException("Material QVM ends before DefineGameMaterial");
    }
    if (parsed_definitions.empty())
    {
        throw logging::ScriptException("Material QVM does not define any material rows");
    }

    definitions_.swap(parsed_definitions);
    definition_by_material_index_.swap(parsed_definition_by_material_index);
    return definitions_.size();
}

void GameMaterialRegistry::GameMaterialRegistry_Clear() noexcept
{
    definitions_.clear();
    definition_by_material_index_.clear();
}

const GameMaterialSourceDefinition* GameMaterialRegistry::GameMaterialRegistry_Find(
    std::size_t material_index) const noexcept
{
    const auto definition_iterator = definition_by_material_index_.find(material_index);
    if (definition_iterator == definition_by_material_index_.end())
    {
        return nullptr;
    }

    return &definitions_[definition_iterator->second];
}

std::size_t GameMaterialRegistry::GameMaterialRegistry_Count() const noexcept
{
    return definitions_.size();
}

const std::vector<GameMaterialSourceDefinition>&
GameMaterialRegistry::GameMaterialRegistry_Definitions() const noexcept
{
    return definitions_;
}

} // namespace igi::assets
