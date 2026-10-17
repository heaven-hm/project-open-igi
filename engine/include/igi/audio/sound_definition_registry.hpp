#pragma once

#include "igi/qvm_initializer.hpp"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace igi::audio
{

enum class SoundDefinitionKind
{
    sound,
    group,
    graph,
    trigger_once,
};

struct SoundDefinition final
{
    std::string name;
    std::string bank_key;
    SoundDefinitionKind kind = SoundDefinitionKind::sound;
    float offset_x{};
    float offset_y{};
    float offset_z{};
    float volume = 1.0F;
    float minimum_distance_metres{};
    float maximum_distance_metres = 1.0F;
    int volume_parameter_index = -1;
    int pitch_parameter_index = -1;
    int bank_key_parameter_index = -1;
    bool looping{};
    std::vector<std::unique_ptr<SoundDefinition>> children;

    [[nodiscard]] bool SoundSystem_IsPlayable() const noexcept
    {
        return kind == SoundDefinitionKind::sound;
    }
};

class SoundDefinitionRegistry final
{
  public:
    // Port loader: the retail QVM names remain exact because they are part of the script ABI.
    [[nodiscard]] std::size_t SoundSystem_LoadForMission(
        const std::filesystem::path& game_directory,
        int mission_number);
    void SoundSystem_Clear() noexcept;
    [[nodiscard]] bool SoundSystem_TryResolve(
        std::string_view name,
        const SoundDefinition*& definition) const;
    [[nodiscard]] std::size_t SoundSystem_Count() const noexcept;
    [[nodiscard]] const std::vector<std::unique_ptr<SoundDefinition>>& SoundSystem_Roots() const noexcept;

  private:
    using DefinitionList = std::vector<std::unique_ptr<SoundDefinition>>;

    [[nodiscard]] std::size_t SoundSystem_LoadScript(const std::filesystem::path& script_path);
    void SoundSystem_AppendScript(const std::filesystem::path& script_path);
    void SoundSystem_ProcessCallTree(
        const std::vector<assets::ScriptCall>& calls,
        std::size_t call_index,
        DefinitionList* parent,
        std::vector<bool>& processed,
        std::size_t recursion_depth);
    void SoundSystem_AddDefinition(std::unique_ptr<SoundDefinition> definition, DefinitionList* parent);

    DefinitionList roots_;
    std::unordered_map<std::string, const SoundDefinition*> definitions_by_name_;
    std::size_t definition_count_{};
};

} // namespace igi::audio
