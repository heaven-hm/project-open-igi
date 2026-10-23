#pragma once

// The retail player-profile records of config.qvm and the GO_* natives that fill them.
//
// igi.exe keeps one 0x564-byte record per profile inside a single global region:
//
//   0x00BC2380  profile count, zeroed by GOStart and incremented by GO_Player
//   0x00BC2384  active profile index, stored by GOStart from its single argument
//   0x00BC2358  record block base; the block of 1-based record c starts at
//               0x00BC2358 + 0x564 * c and holds
//                 +0x000 dword action count (bindings seen for this record)
//                 +0x004 dword mouse invert flag          (GO_InMouInv)
//                 +0x008 float mouse sensitivity          (GO_InMouSens)
//                 +0x00C char[32] profile name            (GO_Player, Sys_ResolveSymbol_23)
//                 +0x02C dword active mission, default 1  (GO_Player, GO_ActiveMission)
//   0x00BC25BC  remap entry base; the entries of 1-based record c start at
//               0x00BC25BC + 0x564 * (c - 1) and hold 64 entries of three dwords.
//               GO_InRemap writes entry (c - 1) * 115 + action.
//   0x00BC79C8  char[32] content-control password (GO_ContentControlPW).
//
// Because 0x00BC25BC + 0x300 == 0x00BC28BC == 0x00BC2358 + 0x564, record c's entry array is
// immediately followed by record c's block, and the block's action-count dword is the same
// dword GO_InRemap reads through the 345-dword counter stride.
//
// The host mirror keeps the retail offsets in two arrays (the counter/block array at
// 0x00BC2358 and the entry array at 0x00BC25BC) because the retail region is one allocation
// and the port's arrays are separate; every offset and stride below is the retail one.

#include <cstddef>
#include <cstdint>

#include "igi/retail/config/input_remap.hpp"

namespace igi::retail::config {

// Retail globals of the profile region.
inline constexpr std::uint32_t profile_count_va = 0x00BC2380U;
inline constexpr std::uint32_t active_profile_index_va = 0x00BC2384U;
inline constexpr std::uint32_t record_block_va = 0x00BC2358U;
inline constexpr std::uint32_t remap_entry_va = 0x00BC25BCU;
inline constexpr std::uint32_t active_record_accessor_va = 0x00BC2388U;
inline constexpr std::uint32_t content_control_password_va = 0x00BC79C8U;

// Offsets inside one record block, in dwords.
inline constexpr std::uint32_t record_action_count_offset = 0U;
inline constexpr std::uint32_t record_mouse_invert_offset = 1U;
inline constexpr std::uint32_t record_mouse_sensitivity_offset = 2U;
inline constexpr std::uint32_t record_name_offset = 3U;
inline constexpr std::uint32_t record_active_mission_offset = 11U;

// 0x00BC2388 - 0x00BC2358, the base Config_GetActivePlayerProfileRecord adds to the stride.
inline constexpr std::uint32_t active_record_accessor_offset = 12U;

// The name and password buffers Sys_ResolveSymbol_23 fills (0x20 bytes each).
inline constexpr std::size_t profile_name_capacity = 0x20U;
inline constexpr std::size_t content_control_password_capacity = 0x20U;

// 0x00BC2384 - 0x00BC2380, the distance between the count and the active index.
inline constexpr std::uint32_t active_index_offset = 1U;

} // namespace igi::retail::config

extern "C" {

// The profile count at 0x00BC2380. GOStart zeroes it, GO_Player increments it and GO_InRemap
// reads it to find the action counter and the entry page of the record being defined.
extern std::uint32_t Config_ProfileCount;

// The retail action count name used by the native differential harnesses; it is the same
// global as Config_ProfileCount (0x00BC2380).
extern std::uint32_t& InputRemap_ActionCount;

// The active profile index at 0x00BC2384.
extern std::uint32_t Config_ActiveProfileIndex;

// The content-control password at 0x00BC79C8.
extern char Config_ContentControlPassword[igi::retail::config::content_control_password_capacity];

// Native: GOStart; retail VA 0x00403660; signature x86 cdecl void*(void* frame).
// Behavior: zeroes the profile count at 0x00BC2380, stores Script_GetArg(frame, 0) (0x004B8A20)
// into 0x00BC2384, and returns the six-dword value block
// {0, 0, 0, 0, 0x00567C74, uninitialised} the caller copies to its destination.
std::uint32_t* GOStart(const igi::retail::config::QvmFrameView* frame);

// Native: GO_Player; retail VA 0x004036B0; signature x86 cdecl void*(void* frame, const char* name).
// Behavior: increments the profile count at 0x00BC2380, zeroes the new record's action count
// (0x00BC2358 + 0x564 * count), writes the default active mission from the constant-1 helper
// at 0x00417090 into (&0x00BC2384)[count * 345], copies the profile name with
// Sys_ResolveSymbol_23 (0x004B8A80) into 0x00BC2364 + 0x564 * count with a 0x20-byte limit,
// and returns the value block.
// Deviation: retail resolves the name from the frame; the host interface takes it resolved
// because the port has no script VM at this boundary.
std::uint32_t* GOPlayer(const igi::retail::config::QvmFrameView* frame, const char* name);

// Native: GO_ActiveMission; retail VA 0x00403750; signature x86 cdecl void*(void* frame).
// Behavior: stores Script_GetArg(frame, 0) (0x004B8A20) into (&0x00BC2384)[count * 345] and
// returns the block.
std::uint32_t* GOActiveMission(const igi::retail::config::QvmFrameView* frame);

// Native: GO_InMouInv; retail VA 0x00403870; signature x86 cdecl void*(void* frame).
// Behavior: stores Script_GetArg(frame, 0) (0x004B8A20) at 0x00BC235C + 0x564 * count and
// returns the block.
std::uint32_t* GOInMouInv(const igi::retail::config::QvmFrameView* frame);

// Native: GO_InMouSens; retail VA 0x004038E0; signature x86 cdecl void*(void* frame).
// Behavior: stores the real returned by Profile_ValueSet(frame, 0) (0x004B8A50) at
// 0x00BC2360 + 0x564 * count and returns the block.
std::uint32_t* GOInMouSens(const igi::retail::config::QvmFrameView* frame);

// Native: GO_ContentControlPW; retail VA 0x00403C10; signature x86 cdecl void*(void* frame, const char* password).
// Behavior: resolves the argument with Sys_ResolveSymbol_23 (0x004B8A80) into the 0x00BC79C8
// buffer with a 0x20-byte limit and returns the block. Deviation: the host interface takes the
// resolved text.
std::uint32_t* GOContentControlPW(const igi::retail::config::QvmFrameView* frame, const char* password);

// Native: Config_GetActivePlayerProfileRecord; retail VA 0x00406220; signature x86 cdecl void*(void).
// Behavior: returns &(&0x00BC2388)[active * 345], the active record's accessor pointer.
std::uint32_t* Config_GetActivePlayerProfileRecord();

// Native: Config_GetActivePlayerProfileIndex; retail VA 0x00405010; signature x86 cdecl void(void* context).
// Behavior: writes the value block {index, uninitialised, double(index), 0x00567C74} to the context.
void Config_GetActivePlayerProfileIndex(void* context);

// Native: Config_SetActivePlayerProfileIndex; retail VA 0x00405050; signature x86 cdecl void(void* context, const void* script_value).
// Behavior: stores Script_GetArg(frame, 0) (0x004B8A20) into 0x00BC2384 and writes the block
// {1, 0, 1.0, type}.
void Config_SetActivePlayerProfileIndex(void* context, std::int32_t index);

} // extern "C"
