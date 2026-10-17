#pragma once

#include "igi/qvm_initializer.hpp"

#include <cstddef>
#include <filesystem>
#include <span>
#include <unordered_map>
#include <vector>

namespace igi::assets
{

struct GameMaterialSourceDefinition final
{
    std::size_t material_index{};
    std::vector<ScriptValue> source_arguments;
};

class GameMaterialRegistry final
{
  public:
    [[nodiscard]] std::size_t GameMaterialRegistry_LoadForGame(
        const std::filesystem::path& game_directory);
    [[nodiscard]] std::size_t GameMaterialRegistry_LoadScript(
        const std::filesystem::path& script_path);
    [[nodiscard]] std::size_t GameMaterialRegistry_ParseCalls(
        std::span<const ScriptCall> script_calls);
    void GameMaterialRegistry_Clear() noexcept;

    [[nodiscard]] const GameMaterialSourceDefinition* GameMaterialRegistry_Find(
        std::size_t material_index) const noexcept;
    [[nodiscard]] std::size_t GameMaterialRegistry_Count() const noexcept;
    [[nodiscard]] const std::vector<GameMaterialSourceDefinition>&
    GameMaterialRegistry_Definitions() const noexcept;

  private:
    std::vector<GameMaterialSourceDefinition> definitions_;
    std::unordered_map<std::size_t, std::size_t> definition_by_material_index_;
};

} // namespace igi::assets
