#include "igi/terrain_surface.hpp"

#include <cmath>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
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

void verify_level_one_terrain_source_arguments(const std::filesystem::path& game_directory)
{
    const std::filesystem::path mission_directory =
        game_directory / "missions" / "location0" / "level1";
    const std::vector<igi::assets::ScriptCall> no_task_overrides;
    const igi::assets::TerrainSurface terrain_surface(mission_directory, no_task_overrides);

    for (int terrain_material_index = 0; terrain_material_index < 6; ++terrain_material_index)
    {
        const igi::assets::ScriptValue* source_argument =
            terrain_surface.TerrainSurface_CreateTerrainMaterialArgumentOne(terrain_material_index);
        require(source_argument != nullptr,
                "Level 1 must retain CreateTerrainMaterial argument 1 for each authored terrain row");
        require(std::isfinite(source_argument->number) && source_argument->number == 1.0,
                "Level 1 terrain row argument 1 must retain its source numeric value");
        require(source_argument->text.empty(),
                "Level 1 terrain row argument 1 must remain a numeric QVM source value");
    }

    require(terrain_surface.TerrainSurface_CreateTerrainMaterialArgumentOne(6) == nullptr,
            "A terrain row absent from the Level 1 QVM must not produce a fabricated source argument");
    require(terrain_surface.TerrainSurface_CreateTerrainMaterialArgumentOne(-1) == nullptr,
            "A negative terrain material index must return no source argument");
}

} // namespace

int main(int argument_count, char** arguments)
{
    try
    {
        if (argument_count != 2)
        {
            throw std::invalid_argument("Expected the original IGI game directory as the only argument");
        }

        verify_level_one_terrain_source_arguments(arguments[1]);
        std::cout << "Level 1 terrain source material checks passed\n";
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Level 1 terrain source material check failed: " << exception.what() << '\n';
        return 1;
    }
}
