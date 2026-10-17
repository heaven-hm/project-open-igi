#include "igi/retail/core/app_context.hpp"

namespace igi::retail::core {
namespace {

AppContextState app_context_state{};

} // namespace

AppContextState& AppContext_Get()
{
    return app_context_state;
}

// 0x0048F1A0  App_ContextSetDebugged
void App_ContextSetDebugged(std::int8_t state)
{
    app_context_state.debugged = static_cast<std::int32_t>(state);
}

// 0x0048F1B0
std::int32_t App_ContextGetDebugged()
{
    return app_context_state.debugged;
}

// 0x0048F1E0  App_ContextSetDebugtextState
void App_ContextSetDebugtextState(std::int8_t state)
{
    app_context_state.debug_text_state = static_cast<std::int32_t>(state);
}

// 0x0048F240  App_ContextSetLightmapsUsed
void App_ContextSetLightmapsUsed(std::int8_t used)
{
    app_context_state.lightmaps_used = static_cast<std::int32_t>(used);
}

// 0x0048F230
std::int32_t App_ContextGetLightmapsUsed()
{
    return app_context_state.lightmaps_used;
}

// 0x0048F260  App_ContextSetTerrainLightmapsUsed
void App_ContextSetTerrainLightmapsUsed(std::int8_t used)
{
    app_context_state.terrain_lightmaps_used = static_cast<std::int32_t>(used);
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
