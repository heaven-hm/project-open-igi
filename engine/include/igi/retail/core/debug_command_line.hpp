#pragma once

#include <cstddef>
#include <span>
#include <string_view>

namespace igi::retail::core {

// Debug-feature command-line switches.
//
// PROVENANCE — read this before changing anything here.
//
// The four setter functions and the six globals below ARE retail IGI.EXE code (verified by
// address and by decompilation). The *command-line switch names themselves are NOT*: the
// strings "DebugKeys", "NoLightmaps", "NoTerrainLightmaps" and "Borderless" do not occur
// anywhere in igi.exe. They are introduced by BlankName's IGIPatch, which hooks the retail
// command-line parser (IGIPatchDLL/patches/_code_mainhooks.asm, "DebugFeaturesPatch") and
// registers:
//
//   proc Main_ParseDebugFeatPatchCmds
//        ccall   ebx,cstrNoLightmaps,Main_ParseNoLightmapsCB
//        ccall   ebx,cstrNoTerrainLightmap,Main_ParseNoTerrainLightmapCB
//        ccall   ebx,cstrDebugText,Main_ParseDebugTextCB
//        ccall   ebx,cstrDebug,Main_ParseDebugCB
//        ccall   ebx,cstrFixmeSmall,Main_ParseSmallCB
//        ccall   ebx,cstrDebugKeys,Main_ParseDebugKeysCB
//
// Each callback then calls a real retail setter:
//   NoLightmaps        -> App_ContextSetLightmapsUsed(1)          (0x0048F240 -> global 0x005C8BE4)
//   NoTerrainLightmaps -> App_ContextSetTerrainLightmapsUsed(1)   (0x0048F260 -> global 0x005C8BE8)
//   DebugText          -> App_ContextSetDebugtextState(1)         (0x0048F1E0 -> global 0x005C8BF4)
//   Debug              -> App_ContextSetDebugged(1)               (0x0048F1A0 -> global 0x005C8BF8)
//   Small              -> AppContext_isFixmeSmall = 1             (global 0x005C8E00)
//   DebugKeys          -> AppContext_SetDebugKeysState(1)         (global 0x0057B194)
//
// Therefore the parser in this file is an explicitly-labelled HOST ADAPTER that reproduces
// IGIPatch's accepted spellings and calls only the real retail setters. It is not a retail
// native and must not be presented as one.
namespace debug_command_line {

// Switch spellings accepted by IGIPatch (its cstr* values); the port accepts the same ones.
inline constexpr std::string_view no_lightmaps = "NoLightmaps";
inline constexpr std::string_view no_terrain_lightmaps = "NoTerrainLightmaps";
inline constexpr std::string_view debug_text = "DebugText";
inline constexpr std::string_view debug = "Debug";
inline constexpr std::string_view small = "Small";
inline constexpr std::string_view debug_keys = "DebugKeys";

// IGIPatch Main_InitDebugFeatPatchCmds: clears the debug-key state before parsing.
// Host adapter (not a retail native).
void Main_InitDebugFeatPatchCmds();

// IGIPatch Main_ParseDebugFeatPatchCmds applied to one token.
// Host adapter (not a retail native). Returns true when the token is a recognised switch.
[[nodiscard]] bool Main_ParseDebugFeatPatchCmds(std::string_view argument);

// Host convenience over the two calls above. Returns the number of recognised switches.
[[nodiscard]] std::size_t ApplyDebugFeatureCommandLine(std::span<const std::string_view> arguments);

} // namespace debug_command_line

} // namespace igi::retail::core
