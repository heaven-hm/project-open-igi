#include "igi/game_material_registry.hpp"

#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{

void require(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

igi::assets::ScriptValue material_index_value(double material_index)
{
    igi::assets::ScriptValue source_value;
    source_value.number = material_index;
    return source_value;
}

igi::assets::ScriptValue material_name_value(std::string material_name)
{
    igi::assets::ScriptValue source_value;
    source_value.text = std::move(material_name);
    return source_value;
}

std::vector<igi::assets::ScriptCall> valid_source_calls()
{
    igi::assets::ScriptCall select_material;
    select_material.function = "SetActiveMaterial";
    select_material.arguments.push_back(material_index_value(7));

    igi::assets::ScriptCall define_render_material;
    define_render_material.function = "DefineQMaterial";

    igi::assets::ScriptCall define_game_material;
    define_game_material.function = "DefineGameMaterial";
    define_game_material.arguments.push_back(material_name_value("TestMaterial"));
    define_game_material.arguments.push_back(material_index_value(10));

    return {select_material, define_render_material, define_game_material};
}

void verify_invalid_source_calls_are_rejected()
{
    igi::assets::GameMaterialRegistry registry;
    const auto valid_calls = valid_source_calls();
    require(registry.GameMaterialRegistry_ParseCalls(valid_calls) == 1,
            "One SetActiveMaterial / DefineGameMaterial pair must produce one row");

    std::vector<igi::assets::ScriptCall> missing_definition_calls;
    igi::assets::ScriptCall select_material;
    select_material.function = "SetActiveMaterial";
    select_material.arguments.push_back(material_index_value(1));
    missing_definition_calls.push_back(select_material);

    bool missing_definition_rejected = false;
    try
    {
        static_cast<void>(registry.GameMaterialRegistry_ParseCalls(missing_definition_calls));
    }
    catch (const std::exception&)
    {
        missing_definition_rejected = true;
    }
    require(missing_definition_rejected, "An incomplete material row must be rejected");
    require(registry.GameMaterialRegistry_Count() == 1,
            "A failed parse must preserve the last complete material table");
    require(registry.GameMaterialRegistry_Find(7) != nullptr &&
                registry.GameMaterialRegistry_Find(7)->source_arguments.front().text ==
                    "TestMaterial",
            "A failed parse must not replace the last complete material table");

    std::vector<igi::assets::ScriptCall> invalid_index_calls;
    select_material.arguments.front() = material_index_value(std::numeric_limits<double>::infinity());
    invalid_index_calls.push_back(select_material);

    bool invalid_index_rejected = false;
    try
    {
        static_cast<void>(registry.GameMaterialRegistry_ParseCalls(invalid_index_calls));
    }
    catch (const std::exception&)
    {
        invalid_index_rejected = true;
    }
    require(invalid_index_rejected, "A non-finite material index must be rejected");
}

void verify_retail_material_script(const std::filesystem::path& game_directory)
{
    igi::assets::GameMaterialRegistry registry;
    const std::size_t loaded_material_count =
        registry.GameMaterialRegistry_LoadForGame(game_directory);
    require(loaded_material_count == 24, "Retail material QVM must define 24 indexed rows");

    const auto* air_material = registry.GameMaterialRegistry_Find(0);
    const auto* ground_material = registry.GameMaterialRegistry_Find(1);
    const auto* final_material = registry.GameMaterialRegistry_Find(23);
    require(air_material != nullptr && air_material->source_arguments.front().text == "Air",
            "Retail material row 0 must preserve its QVM name Air");
    require(ground_material != nullptr && ground_material->source_arguments.front().text == "Ground",
            "Retail material row 1 must preserve its QVM name Ground");
    require(final_material != nullptr && final_material->source_arguments.front().text == "MetalFence",
            "Retail material row 23 must preserve its QVM name MetalFence");

    const auto& ground_arguments = ground_material->source_arguments;
    require(ground_arguments.size() == 36,
            "Retail Ground definition must preserve every source argument without reinterpretation");
    require(ground_arguments[6].text == "walk_ground_1" &&
                ground_arguments[11].text == "walk_ground_6",
            "Retail Ground source values must retain the first and sixth authored footstep strings");
    require(ground_arguments[12].text == "land_ground_1" &&
                ground_arguments[13].text == "rain_2",
            "Retail Ground source values must retain its authored landing and rain strings");
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        verify_invalid_source_calls_are_rejected();
        if (argument_count > 1)
        {
            verify_retail_material_script(arguments[1]);
        }

        std::cout << "Game material registry checks passed\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Game material registry check failed: " << exception.what() << '\n';
        return 1;
    }
}
