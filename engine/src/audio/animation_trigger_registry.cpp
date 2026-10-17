#include "igi/audio/animation_trigger_registry.hpp"

#include "igi/asset_io.hpp"
#include "igi/diagnostics.hpp"
#include "igi/exceptions.hpp"
#include "igi/qvm_initializer.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace igi::audio
{
namespace
{

constexpr std::string_view define_animation_trigger_native = "DefineAnimTrigger";
constexpr std::string_view animation_sound_task_type_identifier = "TASKTYPE_ANIMSOUND";

const assets::ScriptValue& AnimationTriggerRegistry_RequireArgument(
    const assets::ScriptCall& call,
    std::size_t argument_index)
{
    if (argument_index >= call.arguments.size())
    {
        throw logging::ScriptException(
            call.function + " is missing argument " + std::to_string(argument_index));
    }
    return call.arguments[argument_index];
}

std::string AnimationTriggerRegistry_ReadTextArgument(
    const assets::ScriptCall& call,
    std::size_t argument_index)
{
    const auto& argument = AnimationTriggerRegistry_RequireArgument(call, argument_index);
    if (argument.call_index != std::numeric_limits<std::size_t>::max() || argument.text.empty())
    {
        throw logging::ScriptException(
            call.function + " has an invalid text argument " + std::to_string(argument_index));
    }
    return argument.text;
}

std::int32_t AnimationTriggerRegistry_ReadEventIdentifier(const assets::ScriptCall& call)
{
    const auto& argument = AnimationTriggerRegistry_RequireArgument(call, 0);
    if (argument.call_index != std::numeric_limits<std::size_t>::max() || !argument.text.empty() ||
        !std::isfinite(argument.number) || std::trunc(argument.number) != argument.number ||
        argument.number < std::numeric_limits<std::int32_t>::min() ||
        argument.number > std::numeric_limits<std::int32_t>::max())
    {
        throw logging::ScriptException(call.function + " has an invalid event identifier");
    }
    return static_cast<std::int32_t>(argument.number);
}

} // namespace

std::size_t AnimationTriggerRegistry::AnimationTriggerRegistry_LoadForGame(
    const std::filesystem::path& game_directory)
{
    if (game_directory.empty())
    {
        throw logging::ScriptException("Animation trigger load received an empty game directory");
    }

    const std::filesystem::path script_path =
        assets::resolve_path(game_directory, "animtrigger/animtrigger.qvm");
    try
    {
        const std::size_t definition_count = AnimationTriggerRegistry_LoadScript(script_path);
        diagnostics::log(
            diagnostics::Severity::information,
            "animation-audio",
            "Loaded " + std::to_string(definition_count) +
                " raw DefineAnimTrigger entries from " + script_path.string());
        return definition_count;
    }
    catch (const std::exception& exception)
    {
        diagnostics::log(
            diagnostics::Severity::error,
            "animation-audio",
            "Cannot load retail animation trigger definitions from " + script_path.string() + ": " +
                exception.what());
        throw;
    }
}

void AnimationTriggerRegistry::AnimationTriggerRegistry_Clear() noexcept
{
    definitions_.clear();
    definitions_by_event_identifier_.clear();
}

bool AnimationTriggerRegistry::AnimationTriggerRegistry_TryResolve(
    std::int32_t event_identifier,
    const AnimationTriggerDefinition*& definition) const noexcept
{
    definition = nullptr;
    const auto iterator = definitions_by_event_identifier_.find(event_identifier);
    if (iterator == definitions_by_event_identifier_.end())
    {
        return false;
    }

    definition = &definitions_[iterator->second];
    return true;
}

std::size_t AnimationTriggerRegistry::AnimationTriggerRegistry_Count() const noexcept
{
    return definitions_.size();
}

const std::vector<AnimationTriggerDefinition>&
AnimationTriggerRegistry::AnimationTriggerRegistry_Definitions() const noexcept
{
    return definitions_;
}

std::size_t AnimationTriggerRegistry::AnimationTriggerRegistry_LoadScript(
    const std::filesystem::path& script_path)
{
    const assets::FileBytes script_bytes = assets::read_file(script_path);
    const assets::QvmInitializer initializer{assets::BinaryView(script_bytes)};
    std::vector<AnimationTriggerDefinition> definitions;
    std::unordered_map<std::int32_t, std::size_t> definitions_by_event_identifier;
    definitions.reserve(initializer.calls().size());
    definitions_by_event_identifier.reserve(initializer.calls().size());

    for (const auto& call : initializer.calls())
    {
        if (call.function != define_animation_trigger_native || call.arguments.size() != 4)
        {
            throw logging::ScriptException(
                "Retail animation trigger QVM contains an unexpected native or argument count");
        }

        AnimationTriggerDefinition definition;
        definition.event_identifier = AnimationTriggerRegistry_ReadEventIdentifier(call);
        definition.task_type_identifier = AnimationTriggerRegistry_ReadTextArgument(call, 1);
        if (definition.task_type_identifier != animation_sound_task_type_identifier)
        {
            throw logging::ScriptException(
                "Retail animation trigger QVM contains an unsupported symbolic task type: " +
                definition.task_type_identifier);
        }
        definition.sound_definition_name = AnimationTriggerRegistry_ReadTextArgument(call, 2);

        const auto& fourth_argument = AnimationTriggerRegistry_RequireArgument(call, 3);
        if (fourth_argument.call_index != std::numeric_limits<std::size_t>::max() ||
            (fourth_argument.text != "TRUE" && fourth_argument.text != "FALSE") ||
            !std::isfinite(fourth_argument.number))
        {
            throw logging::ScriptException("Retail animation trigger QVM has an invalid fourth argument");
        }
        definition.fourth_argument_text = fourth_argument.text;
        definition.fourth_argument_number = fourth_argument.number;

        const std::size_t definition_index = definitions.size();
        if (!definitions_by_event_identifier.emplace(definition.event_identifier, definition_index).second)
        {
            throw logging::ScriptException(
                "Retail animation trigger QVM repeats event identifier " +
                std::to_string(definition.event_identifier));
        }
        definitions.push_back(std::move(definition));
    }

    if (definitions.empty())
    {
        throw logging::ScriptException("Retail animation trigger QVM contains no definitions");
    }

    definitions_ = std::move(definitions);
    definitions_by_event_identifier_ = std::move(definitions_by_event_identifier);
    return definitions_.size();
}

} // namespace igi::audio
