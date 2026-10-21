#include "igi/retail/core/app_context.hpp"

namespace igi::retail::core {
namespace {

AppContextState app_context_state{};

} // namespace

AppContextState& AppContext_Get()
{
    return app_context_state;
}

// Integer return exposes observed retail EAX; original source return type is unknown.
// 0x0048F1A0  App_ContextSetDebugged
std::int32_t App_ContextSetDebugged(std::int8_t state)
{
    app_context_state.debugged = static_cast<std::int32_t>(state);
    return app_context_state.debugged;
}

// Native: App_ContextGetDebugged; retail VA 0x0048F1B0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F1B0..0x0048F1B7.
// Behavior/CFG: the retail loads the 32-bit debug flag at 0x005C8BF8 and returns it in EAX. The candidate reads the same field of the app context state and returns it.
// Status/limits: STRUCTURALLY_VERIFIED; the load and the EAX return match the retail exactly.
std::int32_t App_ContextGetDebugged()
{
    return app_context_state.debugged;
}

// 0x0048F1E0  App_ContextSetDebugtextState
std::int32_t App_ContextSetDebugtextState(std::int8_t state)
{
    app_context_state.debug_text_state = static_cast<std::int32_t>(state);
    return app_context_state.debug_text_state;
}

// 0x0048F240  App_ContextSetLightmapsUsed
std::int32_t App_ContextSetLightmapsUsed(std::int8_t used)
{
    app_context_state.lightmaps_used = static_cast<std::int32_t>(used);
    return app_context_state.lightmaps_used;
}

// 0x0048F230
std::int32_t App_ContextGetLightmapsUsed()
{
    return app_context_state.lightmaps_used;
}

// 0x0048F260  App_ContextSetTerrainLightmapsUsed
std::int32_t App_ContextSetTerrainLightmapsUsed(std::int8_t used)
{
    app_context_state.terrain_lightmaps_used = static_cast<std::int32_t>(used);
    return app_context_state.terrain_lightmaps_used;
}

// 0x0048F250
std::int32_t App_ContextGetTerrainLightmapsUsed()
{
    return app_context_state.terrain_lightmaps_used;
}

// 0x0057B194 GameFunctions_isEnableDebugKeys.
void GameFunctions_SetEnableDebugKeys(std::int32_t enabled)
{
    app_context_state.debug_keys_enabled = enabled;
}

std::int32_t GameFunctions_GetEnableDebugKeys()
{
    return app_context_state.debug_keys_enabled;
}

// IGIPatch AppContext_SetDebugKeysState.
void AppContext_SetDebugKeysState(std::int32_t enabled)
{
    GameFunctions_SetEnableDebugKeys(enabled);
}

// 0x005C8E00 AppContext_isFixmeSmall.
void AppContext_SetFixmeSmall(std::int8_t enabled)
{
    app_context_state.fixme_small = static_cast<std::int32_t>(enabled);
}

std::int32_t AppContext_GetFixmeSmall()
{
    return app_context_state.fixme_small;
}

} // namespace igi::retail::core
