#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace igi::audio
{

// Port-side source table for the retail DefineAnimTrigger native registered at 0x004ebe70.
// Callback behavior at 0x004ebeb0 is not fully verified, so all non-ID fields remain source values.
struct AnimationTriggerDefinition final
{
    std::int32_t event_identifier{};
    std::string task_type_identifier;
    std::string sound_definition_name;
    std::string fourth_argument_text;
    double fourth_argument_number{};
};

class AnimationTriggerRegistry final
{
  public:
    [[nodiscard]] std::size_t AnimationTriggerRegistry_LoadForGame(
        const std::filesystem::path& game_directory);
    void AnimationTriggerRegistry_Clear() noexcept;
    [[nodiscard]] bool AnimationTriggerRegistry_TryResolve(
        std::int32_t event_identifier,
        const AnimationTriggerDefinition*& definition) const noexcept;
    [[nodiscard]] std::size_t AnimationTriggerRegistry_Count() const noexcept;
    [[nodiscard]] const std::vector<AnimationTriggerDefinition>&
    AnimationTriggerRegistry_Definitions() const noexcept;

  private:
    [[nodiscard]] std::size_t AnimationTriggerRegistry_LoadScript(const std::filesystem::path& script_path);

    std::vector<AnimationTriggerDefinition> definitions_;
    std::unordered_map<std::int32_t, std::size_t> definitions_by_event_identifier_;
};

} // namespace igi::audio
