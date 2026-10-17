#pragma once

#include <cstdint>

namespace igi::retail::core {

// AppContext state globals.
//
// These are the retail IGI.EXE globals (image base 0x00400000). Each accessor is a
// transcription of the retail function at the address listed; the globals are 32-bit and the
// setters widen the int8_t parameter into them.
//
//   0x005C8BF8  object/debug flag        set 0x0048F1A0   get 0x0048F1B0
//   0x005C8BE4  lightmaps used           set 0x0048F240   get 0x0048F230
//   0x005C8BE8  terrain lightmaps used   set 0x0048F260   get 0x0048F250
//   0x005C8BF4  debug-text state         set 0x0048F1E0   get 0x0048F2xx (see note below)
//   0x0057B194  debug keys enabled       set 0x0048F2F0   (IGIPatch GameFunctions_SetEnableDebugKeys)
//   0x005C8E00  fixme-small              set by IGIPatch Main_ParseSmallCB
//
// The supported retail setters are `movsx` + `mov dword`; none of them interprets the value, so
// the port stores it unchanged. What 1 and 0 mean is decided by the retail consumer of each
// global, which is a separate function - the port must not assign meaning here.
struct AppContextState {
    std::int32_t debugged = 0;
    std::int32_t debug_text_state = 0;
    std::int32_t lightmaps_used = 0;
    std::int32_t terrain_lightmaps_used = 0;
    std::int32_t debug_keys_enabled = 0;
    std::int32_t fixme_small = 0;
};

[[nodiscard]] AppContextState& AppContext_Get();

// 0x0048F1A0:  movsx eax, byte [esp+4] ; mov dword [0x005C8BF8], eax ; retn
void App_ContextSetDebugged(std::int8_t state);
// 0x0048F1B0:  mov eax, dword [0x005C8BF8] ; retn
[[nodiscard]] std::int32_t App_ContextGetDebugged();

// 0x0048F1E0:  movsx eax, byte [esp+4] ; mov dword [0x005C8BF4], eax ; retn
void App_ContextSetDebugtextState(std::int8_t state);

// 0x0048F240:  movsx eax, byte [esp+4] ; mov dword [0x005C8BE4], eax ; retn
void App_ContextSetLightmapsUsed(std::int8_t used);
// 0x0048F230:  mov eax, dword [0x005C8BE4] ; retn
[[nodiscard]] std::int32_t App_ContextGetLightmapsUsed();

// 0x0048F260:  movsx eax, byte [esp+4] ; mov dword [0x005C8BE8], eax ; retn
void App_ContextSetTerrainLightmapsUsed(std::int8_t used);
// 0x0048F250:  mov eax, dword [0x005C8BE8] ; retn
[[nodiscard]] std::int32_t App_ContextGetTerrainLightmapsUsed();

// 0x0057B194 GameFunctions_isEnableDebugKeys.
void GameFunctions_SetEnableDebugKeys(std::int32_t enabled);
[[nodiscard]] std::int32_t GameFunctions_GetEnableDebugKeys();

// IGIPatch wrapper: sets the retail flag and mirrors AppContext_isDebugKeys.
void AppContext_SetDebugKeysState(std::int32_t enabled);

// 0x005C8E00 AppContext_isFixmeSmall.
void AppContext_SetFixmeSmall(std::int8_t enabled);
[[nodiscard]] std::int32_t AppContext_GetFixmeSmall();

} // namespace igi::retail::core
