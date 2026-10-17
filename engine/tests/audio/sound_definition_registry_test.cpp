#include "igi/audio/sound_definition_registry.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace
{

void require(bool condition, std::string_view message)
{
    if (!condition)
    {
        throw std::runtime_error(std::string(message));
    }
}

const igi::audio::SoundDefinition* find_child(
    const igi::audio::SoundDefinition& parent,
    std::string_view name)
{
    const auto iterator = std::find_if(
        parent.children.begin(),
        parent.children.end(),
        [name](const auto& child) { return child->name == name; });
    return iterator == parent.children.end() ? nullptr : iterator->get();
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count != 2)
        {
            throw std::runtime_error("Usage: sound_definition_registry_test <original-game-directory>");
        }

        igi::audio::SoundDefinitionRegistry registry;
        const std::size_t definition_count = registry.SoundSystem_LoadForMission(arguments[1], 1);
        require(definition_count == 431, "Level 1 must load all 431 common and mission definitions");

        const igi::audio::SoundDefinition* rifle = nullptr;
        require(registry.SoundSystem_TryResolve("ak47_loop", rifle), "AK-47 loop definition is missing");
        require(rifle->bank_key == "AK47_LOOP", "AK-47 definition does not map to the retail bank key");
        require(rifle->looping, "AK-47 loop definition lost its retail loop flag");
        require(rifle->kind == igi::audio::SoundDefinitionKind::sound, "AK-47 definition has the wrong kind");

        const igi::audio::SoundDefinition* guard_death = nullptr;
        require(registry.SoundSystem_TryResolve("ai_death_1", guard_death), "Guard death definition is missing");
        require(guard_death->bank_key == "AI_DEATH_01", "Guard death definition lost its padded bank key");

        const igi::audio::SoundDefinition* mission_ambience = nullptr;
        require(registry.SoundSystem_TryResolve("m1_ambience", mission_ambience), "Level 1 ambience is missing");
        require(mission_ambience->bank_key == "M1_AMBIENCE", "Level 1 ambience bank key is incorrect");

        const auto root_group = std::find_if(
            registry.SoundSystem_Roots().begin(),
            registry.SoundSystem_Roots().end(),
            [](const auto& definition) { return definition->name == "GUN SHOTS"; });
        require(root_group != registry.SoundSystem_Roots().end(), "Retail GUN SHOTS group is missing");
        require((*root_group)->children.size() == 27, "Retail GUN SHOTS child count changed");
        require(find_child(**root_group, "ak47_loop") != nullptr, "AK-47 loop is not nested under GUN SHOTS");

        const igi::audio::SoundDefinition* empty_name = nullptr;
        require(!registry.SoundSystem_TryResolve("", empty_name), "An empty definition name must not resolve");
        require(empty_name == nullptr, "A failed lookup must clear its output pointer");

        std::cout << "Level 1 retail sound definitions passed: " << definition_count
                  << " entries, correct native mappings, group order, and first-match lookup\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Level 1 sound definition verification failed: " << exception.what() << '\n';
        return 1;
    }
}
