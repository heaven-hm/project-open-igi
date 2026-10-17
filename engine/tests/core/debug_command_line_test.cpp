#include "igi/retail/core/app_context.hpp"
#include "igi/retail/core/debug_command_line.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>
#include <vector>

namespace {

void require(bool condition, std::string_view message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void reset_state()
{
    igi::retail::core::AppContextState& state = igi::retail::core::AppContext_Get();
    state.debugged = 0;
    state.debug_text_state = 0;
    state.lightmaps_used = 0;
    state.terrain_lightmaps_used = 0;
    state.debug_keys_enabled = 0;
    state.fixme_small = 0;
}

// IGIPatch Main_ParseDebugKeysCB -> AppContext_SetDebugKeysState -> GameFunctions_isEnableDebugKeys (0x0057B194).
void test_debug_keys_switch_sets_the_retail_flag()
{
    reset_state();
    require(igi::retail::core::debug_command_line::Main_ParseDebugFeatPatchCmds("DebugKeys"),
            "\"DebugKeys\" is a recognised switch");
    require(igi::retail::core::AppContext_Get().debug_keys_enabled == 1,
            "\"DebugKeys\" sets GameFunctions_isEnableDebugKeys (0x0057B194)");
}

// IGIPatch Main_ParseDebugCB / Main_ParseDebugTextCB -> 0x0048F1A0 / 0x0048F1E0.
void test_debug_and_debug_text_switches_match_retail_setters()
{
    reset_state();
    require(igi::retail::core::debug_command_line::Main_ParseDebugFeatPatchCmds("Debug"),
            "\"Debug\" is a recognised switch");
    require(igi::retail::core::AppContext_Get().debugged == 1,
            "\"Debug\" sets the App_ContextSetDebugged flag (0x005C8BF8)");

    require(igi::retail::core::debug_command_line::Main_ParseDebugFeatPatchCmds("DebugText"),
            "\"DebugText\" is a recognised switch");
    require(igi::retail::core::AppContext_Get().debug_text_state == 1,
            "\"DebugText\" sets the App_ContextSetDebugtextState flag (0x005C8BF4)");
}

// IGIPatch Main_ParseNoLightmapsCB / Main_ParseNoTerrainLightmapCB -> 0x0048F240 / 0x0048F260.
// IGIPatch passes TRUE for both of these, and that pairing is reproduced here.
void test_lightmap_switches_match_igipatch_polarity()
{
    reset_state();
    require(igi::retail::core::debug_command_line::Main_ParseDebugFeatPatchCmds("NoLightmaps"),
            "\"NoLightmaps\" is a recognised switch");
    require(igi::retail::core::AppContext_Get().lightmaps_used == 1,
            "IGIPatch calls App_ContextSetLightmapsUsed(TRUE) for NoLightmaps");

    require(igi::retail::core::debug_command_line::Main_ParseDebugFeatPatchCmds("NoTerrainLightmaps"),
            "\"NoTerrainLightmaps\" is a recognised switch");
    require(igi::retail::core::AppContext_Get().terrain_lightmaps_used == 1,
            "IGIPatch calls App_ContextSetTerrainLightmapsUsed(TRUE) for NoTerrainLightmaps");
}

// IGIPatch Main_ParseSmallCB -> AppContext_isFixmeSmall byte at 0x005C8E00.
void test_small_switch_sets_the_fixme_small_byte()
{
    reset_state();
    require(igi::retail::core::debug_command_line::Main_ParseDebugFeatPatchCmds("Small"),
            "\"Small\" is a recognised switch");
    require(igi::retail::core::AppContext_Get().fixme_small == 1,
            "\"Small\" sets AppContext_isFixmeSmall (0x005C8E00)");
}

void test_unknown_switch_is_not_consumed()
{
    reset_state();
    require(!igi::retail::core::debug_command_line::Main_ParseDebugFeatPatchCmds("--res"),
            "host arguments are not mistaken for debug switches");
    require(igi::retail::core::AppContext_Get().debug_keys_enabled == 0,
            "an unrecognised token leaves the debug-key flag clear");
}

// IGIPatch Main_InitDebugFeatPatchCmds clears the debug-key state before parsing.
void test_apply_clears_then_sets()
{
    igi::retail::core::AppContext_Get().debug_keys_enabled = 1;

    const std::vector<std::string_view> arguments{"DebugKeys", "Debug"};
    const std::size_t recognised =
        igi::retail::core::debug_command_line::ApplyDebugFeatureCommandLine(arguments);

    require(recognised == 2U, "two recognised switches were counted");
    require(igi::retail::core::AppContext_Get().debug_keys_enabled == 1, "DebugKeys re-enabled after init");
    require(igi::retail::core::AppContext_Get().debugged == 1, "Debug applied");
}

} // namespace

int main()
{
    test_debug_keys_switch_sets_the_retail_flag();
    test_debug_and_debug_text_switches_match_retail_setters();
    test_lightmap_switches_match_igipatch_polarity();
    test_small_switch_sets_the_fixme_small_byte();
    test_unknown_switch_is_not_consumed();
    test_apply_clears_then_sets();

    std::cout << "All retail debug command-line tests passed.\n";
    return EXIT_SUCCESS;
}
