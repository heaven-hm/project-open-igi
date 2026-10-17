#include "igi/retail/core/debug_command_line.hpp"

#include "igi/retail/core/app_context.hpp"

namespace igi::retail::core::debug_command_line {

// IGIPatch Main_InitDebugFeatPatchCmds:
//     ccall AppContext_SetDebugKeysState,FALSE
void Main_InitDebugFeatPatchCmds()
{
    AppContext_SetDebugKeysState(0);
}

// IGIPatch Main_ParseNoLightmapsCB: push TRUE / call AppContext_SetLightmapsUsed (0x0048F240).
//
// NOTE: IGIPatch passes TRUE for the switch named "NoLightmaps" while the retail global at
// 0x005C8BE4 is read as "lightmaps used". This pairing is reproduced exactly as IGIPatch has
// it; it needs a runtime check against retail before the port relies on the polarity.
namespace {

void Main_ParseNoLightmapsCB()
{
    App_ContextSetLightmapsUsed(1);
}

// IGIPatch Main_ParseNoTerrainLightmapCB: call AppContext_SetTerrainLightmapsUsed (0x0048F260).
void Main_ParseNoTerrainLightmapCB()
{
    App_ContextSetTerrainLightmapsUsed(1);
}

// IGIPatch Main_ParseDebugTextCB: call AppContext_SetDebugtextState (0x0048F1E0).
void Main_ParseDebugTextCB()
{
    App_ContextSetDebugtextState(1);
}

// IGIPatch Main_ParseDebugCB: call AppContext_SetDebugged (0x0048F1A0).
void Main_ParseDebugCB()
{
    App_ContextSetDebugged(1);
}

// IGIPatch Main_ParseSmallCB: mov byte[AppContext_isFixmeSmall],TRUE (global 0x005C8E00).
void Main_ParseSmallCB()
{
    AppContext_SetFixmeSmall(1);
}

// IGIPatch Main_ParseDebugKeysCB: call AppContext_SetDebugKeysState,TRUE (global 0x0057B194).
void Main_ParseDebugKeysCB()
{
    AppContext_SetDebugKeysState(1);
}

} // namespace

bool Main_ParseDebugFeatPatchCmds(std::string_view argument)
{
    if (argument == no_lightmaps) {
        Main_ParseNoLightmapsCB();
        return true;
    }
    if (argument == no_terrain_lightmaps) {
        Main_ParseNoTerrainLightmapCB();
        return true;
    }
    if (argument == debug_text) {
        Main_ParseDebugTextCB();
        return true;
    }
    if (argument == debug) {
        Main_ParseDebugCB();
        return true;
    }
    if (argument == small) {
        Main_ParseSmallCB();
        return true;
    }
    if (argument == debug_keys) {
        Main_ParseDebugKeysCB();
        return true;
    }
    return false;
}

std::size_t ApplyDebugFeatureCommandLine(std::span<const std::string_view> arguments)
{
    Main_InitDebugFeatPatchCmds();

    std::size_t recognised = 0U;
    for (const std::string_view argument : arguments) {
        if (Main_ParseDebugFeatPatchCmds(argument)) {
            ++recognised;
        }
    }
    return recognised;
}

} // namespace igi::retail::core::debug_command_line
