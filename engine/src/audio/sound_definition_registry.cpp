#include "igi/audio/sound_definition_registry.hpp"

#include "igi/asset_io.hpp"
#include "igi/diagnostics.hpp"
#include "igi/exceptions.hpp"
#include "igi/qvm_initializer.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>

namespace igi::audio
{
namespace
{

constexpr std::size_t maximum_sound_definition_depth = 128;
constexpr std::string_view define_sound_native = "DefineSound";
constexpr std::string_view define_group_native = "DefineGroup";
constexpr std::string_view define_graph_native = "DefineGraph";
constexpr std::string_view define_trigger_once_native = "DefineTriggerOnce";

const igi::assets::ScriptValue& SoundSystem_RequireArgument(
    const igi::assets::ScriptCall& call,
    std::size_t argument_index)
{
    if (argument_index >= call.arguments.size())
    {
        throw logging::ScriptException(
            call.function + " is missing argument " + std::to_string(argument_index));
    }
    return call.arguments[argument_index];
}

std::string SoundSystem_ReadStringArgument(
    const igi::assets::ScriptCall& call,
    std::size_t argument_index)
{
    const auto& argument = SoundSystem_RequireArgument(call, argument_index);
    if (argument.call_index != std::numeric_limits<std::size_t>::max())
    {
        throw logging::ScriptException(
            call.function + " has a nested call where a string is required at argument " +
            std::to_string(argument_index));
    }
    return argument.text;
}

float SoundSystem_ReadRealArgument(
    const igi::assets::ScriptCall& call,
    std::size_t argument_index)
{
    const auto& argument = SoundSystem_RequireArgument(call, argument_index);
    if (argument.call_index != std::numeric_limits<std::size_t>::max() || !argument.text.empty() ||
        !std::isfinite(argument.number) ||
        argument.number < -static_cast<double>(std::numeric_limits<float>::max()) ||
        argument.number > static_cast<double>(std::numeric_limits<float>::max()))
    {
        throw logging::ScriptException(
            call.function + " has an invalid real argument " + std::to_string(argument_index));
    }
    return static_cast<float>(argument.number);
}

int SoundSystem_ReadIntegerArgument(
    const igi::assets::ScriptCall& call,
    std::size_t argument_index)
{
    const auto& argument = SoundSystem_RequireArgument(call, argument_index);
    if (argument.call_index == std::numeric_limits<std::size_t>::max() && argument.text == "TRUE")
    {
        return 1;
    }
    if (argument.call_index == std::numeric_limits<std::size_t>::max() && argument.text == "FALSE")
    {
        return 0;
    }

    const double value = SoundSystem_ReadRealArgument(call, argument_index);
    if (std::trunc(value) != value || value < std::numeric_limits<int>::min() ||
        value > std::numeric_limits<int>::max())
    {
        throw logging::ScriptException(
            call.function + " has an invalid integer argument " + std::to_string(argument_index));
    }
    return static_cast<int>(value);
}

} // namespace

void SoundDefinitionRegistry::SoundSystem_Clear() noexcept
{
    roots_.clear();
    definitions_by_name_.clear();
    definition_count_ = 0;
}

bool SoundDefinitionRegistry::SoundSystem_TryResolve(
    std::string_view name,
    const SoundDefinition*& definition) const
{
    definition = nullptr;
    if (name.empty())
    {
        return false;
    }

    const auto iterator = definitions_by_name_.find(std::string(name));
    if (iterator == definitions_by_name_.end())
    {
        return false;
    }
    definition = iterator->second;
    return true;
}

std::size_t SoundDefinitionRegistry::SoundSystem_Count() const noexcept
{
    return definition_count_;
}

const std::vector<std::unique_ptr<SoundDefinition>>& SoundDefinitionRegistry::SoundSystem_Roots() const noexcept
{
    return roots_;
}

std::size_t SoundDefinitionRegistry::SoundSystem_LoadForMission(
    const std::filesystem::path& game_directory,
    int mission_number)
{
    if (game_directory.empty() || mission_number < 1 || mission_number > 14)
    {
        throw logging::ScriptException("Sound definition load received an invalid game path or mission number");
    }

    SoundSystem_Clear();
    const std::size_t before_shared = definition_count_;
    SoundSystem_AppendScript(
        assets::resolve_path(game_directory, "common/sounds/sounds.qvm"));
    const std::size_t shared_count = definition_count_ - before_shared;

    const std::size_t before_mission = definition_count_;
    const std::filesystem::path mission_script = assets::resolve_path(
        game_directory,
        "missions/location0/level" + std::to_string(mission_number) + "/sounds/sounds.qvm");
    SoundSystem_AppendScript(mission_script);
    const std::size_t mission_count = definition_count_ - before_mission;

    diagnostics::log(
        diagnostics::Severity::information,
        "audio",
        "Loaded " + std::to_string(shared_count) + " common and " + std::to_string(mission_count) +
            " mission sound definitions (" + std::to_string(definition_count_) + " total)");
    return definition_count_;
}

void SoundDefinitionRegistry::SoundSystem_AppendScript(const std::filesystem::path& script_path)
{
    try
    {
        const std::size_t loaded_count = SoundSystem_LoadScript(script_path);
        diagnostics::log(
            diagnostics::Severity::debug,
            "audio",
            "Loaded " + std::to_string(loaded_count) + " sound definitions from " + script_path.string());
    }
    catch (const std::exception& exception)
    {
        diagnostics::log(
            diagnostics::Severity::error,
            "audio",
            "Cannot load retail sound definitions from " + script_path.string() + ": " + exception.what());
        throw;
    }
}

std::size_t SoundDefinitionRegistry::SoundSystem_LoadScript(const std::filesystem::path& script_path)
{
    const std::size_t before = definition_count_;
    const assets::FileBytes script_bytes = assets::read_file(script_path);
    const assets::QvmInitializer initializer{assets::BinaryView(script_bytes)};
    const auto& calls = initializer.calls();

    std::vector<bool> nested_calls(calls.size(), false);
    for (const auto& call : calls)
    {
        for (const auto& argument : call.arguments)
        {
            if (argument.call_index == std::numeric_limits<std::size_t>::max())
            {
                continue;
            }
            if (argument.call_index >= calls.size())
            {
                throw logging::ScriptException("Sound QVM references an invalid nested call index");
            }
            nested_calls[argument.call_index] = true;
        }
    }

    std::vector<bool> processed_calls(calls.size(), false);
    for (std::size_t call_index = 0; call_index < calls.size(); ++call_index)
    {
        if (!nested_calls[call_index])
        {
            SoundSystem_ProcessCallTree(calls, call_index, nullptr, processed_calls, 0);
        }
    }

    for (std::size_t call_index = 0; call_index < processed_calls.size(); ++call_index)
    {
        if (!processed_calls[call_index])
        {
            throw logging::ScriptException("Sound QVM contains an unreachable native call");
        }
    }
    return definition_count_ - before;
}

void SoundDefinitionRegistry::SoundSystem_ProcessCallTree(
    const std::vector<assets::ScriptCall>& calls,
    std::size_t call_index,
    DefinitionList* parent,
    std::vector<bool>& processed_calls,
    std::size_t recursion_depth)
{
    if (recursion_depth > maximum_sound_definition_depth || call_index >= calls.size())
    {
        throw logging::ScriptException("Sound definition nesting exceeds the verified limit");
    }
    if (processed_calls[call_index])
    {
        throw logging::ScriptException("Sound QVM references a definition call more than once");
    }
    processed_calls[call_index] = true;

    const auto& call = calls[call_index];
    SoundDefinitionKind kind;
    if (call.function == define_sound_native)
    {
        kind = SoundDefinitionKind::sound;
    }
    else if (call.function == define_group_native)
    {
        kind = SoundDefinitionKind::group;
    }
    else if (call.function == define_graph_native)
    {
        kind = SoundDefinitionKind::graph;
    }
    else if (call.function == define_trigger_once_native)
    {
        kind = SoundDefinitionKind::trigger_once;
    }
    else
    {
        throw logging::ScriptException("Unexpected native in retail sound script: " + call.function);
    }

    auto definition = std::make_unique<SoundDefinition>();
    definition->name = SoundSystem_ReadStringArgument(call, 0);
    definition->kind = kind;

    if (kind == SoundDefinitionKind::group)
    {
        if (call.arguments.empty())
        {
            throw logging::ScriptException("DefineGroup has no group name");
        }
        SoundDefinition* group = definition.get();
        SoundSystem_AddDefinition(std::move(definition), parent);
        for (std::size_t argument_index = 1; argument_index < call.arguments.size(); ++argument_index)
        {
            const std::size_t child_call_index = call.arguments[argument_index].call_index;
            if (child_call_index == std::numeric_limits<std::size_t>::max())
            {
                throw logging::ScriptException("DefineGroup contains a non-call child argument");
            }
            SoundSystem_ProcessCallTree(
                calls,
                child_call_index,
                &group->children,
                processed_calls,
                recursion_depth + 1);
        }
        return;
    }

    // Original IGI.EXE native callbacks: DefineSound (0x004E8C60), DefineGraph (0x004EAD20),
    // DefineTriggerOnce (0x004E9EC0), and DefineGroup (0x004EBA90), registered at 0x004E6070.
    // REA/Ghidra evidence ev_ab05904af5e3cd11172872052c89c9bf62b5c027a695a6db30c955506148ec9a
    // records the retail registration table and the exact QVM symbol-to-callback bindings.
    definition->bank_key = SoundSystem_ReadStringArgument(call, 1);
    definition->offset_x = SoundSystem_ReadRealArgument(call, 2);
    definition->offset_y = SoundSystem_ReadRealArgument(call, 3);
    definition->offset_z = SoundSystem_ReadRealArgument(call, 4);
    definition->volume = SoundSystem_ReadRealArgument(call, 5);
    definition->minimum_distance_metres = SoundSystem_ReadRealArgument(call, 6);
    definition->maximum_distance_metres = SoundSystem_ReadRealArgument(call, 7);

    if (kind == SoundDefinitionKind::sound)
    {
        definition->volume_parameter_index = SoundSystem_ReadIntegerArgument(call, 8);
        definition->pitch_parameter_index = SoundSystem_ReadIntegerArgument(call, 9);
        definition->bank_key_parameter_index = SoundSystem_ReadIntegerArgument(call, 10);
        definition->looping = call.arguments.size() > 12 && SoundSystem_ReadIntegerArgument(call, 12) != 0;
    }

    SoundSystem_AddDefinition(std::move(definition), parent);
}

void SoundDefinitionRegistry::SoundSystem_AddDefinition(
    std::unique_ptr<SoundDefinition> definition,
    DefinitionList* parent)
{
    if (!definition)
    {
        throw logging::ScriptException("Cannot register an empty retail sound definition");
    }

    SoundDefinition* definition_pointer = definition.get();
    if (parent == nullptr)
    {
        roots_.push_back(std::move(definition));
    }
    else
    {
        parent->push_back(std::move(definition));
    }

    ++definition_count_;
    // Retail lookup stops at the first equal name. Preserve the order emitted by QVM, including
    // a group before nested definitions and common definitions before mission overrides.
    definitions_by_name_.try_emplace(definition_pointer->name, definition_pointer);
}

} // namespace igi::audio
