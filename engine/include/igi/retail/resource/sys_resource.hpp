#pragma once

#include <cstddef>
#include <cstdint>

namespace igi::retail::resource {

using SysAllocateMemory5Fn = std::int32_t (*)(std::int32_t* out_meta, const char* path);
using SysOpenFileStream5Fn = std::int32_t (*)(void* data);

// Native: Resource_MatchPrefix() - VA 0x004AAD10 - complete port.
// Behavior: compares up to `max_len` bytes of `str1` (`context`) and `str2` (`arg2`), bounded by
//           the length of `str1` (plus its NUL byte when shorter than `max_len`); returns `-1`
//           if the first differing byte of `str2 > str1`, `1` if `str2 < str1`, or `0` if equal.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x004AAD10-0x004AAD48, 56 bytes, digest 6f465400533e36a49cafc375dc4058fb995240dab60cd989d242464616bf2034;
//                 CLEAN/camera/004AAD10-Camera_ApplyLineFilter_21.c.
// CFG: radare2 6 blocks/8 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Confidence: high for exact bounded prefix comparison (`strncmp`-equivalent).
// Signature: x86 cdecl int32_t(const void* context, const void* arg2, int32_t arg3).
// Usage/tests: called by Resource_RegisterBuffer (0x004B5F80) to test `"RESOURCE"` 8-byte header magic; exercised by sys_resource_test.cpp.
std::int32_t Resource_MatchPrefix(const void* context, const void* arg2, std::int32_t arg3);
inline std::int32_t Camera_ApplyLineFilter_21(const void* context, const void* arg2, std::int32_t arg3)
{
    return Resource_MatchPrefix(context, arg2, arg3);
}

// Native: Resource_CreateNode() - VA 0x004B59F0 - complete port.
// Behavior: allocates a resource table slot from pool `g_943e20` using free-list index table
//           `g_943e24[g_943e28++]` and stride `g_943e30`, records its active index in
//           `*(g_943e34 + result)`, upper-cases `context` into `result + 0x38` (`result[0x0E]`) via
//           Resource_CanonicalizePath (0x004B59C0), normalizes `'\\'` (`0x5C`) to `'/'` (`0x2F`), sets
//           `result[0x0D]` (`+0x34` data pointer) to `arg3 ? arg3 : result`, `result[0x0A]` (`+0x28`
//           size) = `arg4`, `result[0x09]` (`+0x24` kind) = `arg5`, `result[0x0C]` (`+0x30`) = `1`,
//           `result[0x0B]` (`+0x2C` loaded_flag) = `0`, `result[0x8E]` (`+0x238`) = `arg2`,
//           `result[0x08]` (`+0x20` hash) = Resource_CalculatePathHash(`&result[0x0E]`), initializes links
//           `result[0..7]`, links `result` into `g_943e38` (`0x00943E40` hash list) via
//           HeapList_Insert (0x004AF8F0), and copies `arg6[0..1]` into `result[0x8F..0x90]`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B59F0-0x004B5AE9, 249 bytes,
//                 digest d1a0fe269c4734cc2da57044b5771afd3f5ea268e254542d7e62292455455c97;
//                 CLEAN/sys/004B59F0-Resource_CreateNode.c.
// CFG: radare2 10 blocks/13 edges; Binary Ninja 10 blocks; 100% structural agreement.
// Confidence: high for exact slot allocation, path normalization, hash computation, and list insertion.
// Signature: x86 cdecl void*(const char* context, void* arg2, void* arg3, int32_t arg4, int32_t arg5, const int32_t* arg6).
// Usage/tests: inserts top-level and child resource entries during Resource_RegisterBuffer / Resource_RegisterIlffContainer; exercised by sys_resource_test.cpp.
void* Resource_CreateNode(const char* context, void* arg2, void* arg3, std::int32_t arg4,
                          std::int32_t arg5, const std::int32_t* arg6);

// Native: Resource_FreeNode() - VA 0x004B5C90 - complete port.
// Behavior: switches on `context[9]` (`+0x24` resource kind):
//           - case 0 (container): walks child list at `context[5]`, recursively calling
//             Resource_FreeNode on each child node (`i_1[2]`), unlinks `context` via
//             HeapList_Remove (0x004AF910), frees `context[0x0D]` via Sys_OpenFileStream_5
//             (0x004B1A30), and returns the slot to free-list `g_943e24` / `g_943e28`;
//           - case 1, 4 (sub-resource): unlinks child link `&context[2]` and `context`, then returns slot;
//           - case 2 (standalone raw resource): frees `context[0x0D]`, unlinks `context`, returns slot;
//           - case 3: unlinks `context` and returns slot.
//           Return value: the new `g_943e28` count when the freed node was the last active slot;
//           otherwise the address of the node relocated into the freed position by the swap-pop
//           (retail EAX behavior, verified under angr against retail 0x004B5C90 execution).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B5C90-0x004B5D6A, 218 bytes,
//                 digest ff4a8050127977de2b75901a65663b90290f997e9f45732720f5b2c1994cb5b2;
//                 CLEAN/sys/004B5C90-Resource_FreeNode.c.
// CFG: radare2 13 blocks/18 edges; Binary Ninja 13 blocks; 100% structural agreement.
// Confidence: high for exact switch cases, child recursion, list removal, and pool swap-pop.
// Signature: x86 cdecl int32_t(void* context); ported as std::int32_t Resource_FreeNode(void* context).
// Usage/tests: releases resource nodes and returns pool indices; exercised by sys_resource_test.cpp.
std::int32_t Resource_FreeNode(void* context);

// Native: Resource_RegisterIlffContainer() - VA 0x004B60F0 - complete port.
// Behavior: walks ILFF chunks inside an `ILFF`/`IRES` container (`arg3`) starting at
//           Ilff_GetFirstChunk(arg3) (0x004B7490): requires `NAME` (`0x454D414E`) followed by a
//           payload chunk (`BODY` `0x59444F42`, nested `ILFF`/`IRES` `0x46464C49`/`0x53455249`,
//           `CSTR` `0x52545343`, or `PATH` `0x48544150`, with optional `DBUG` `0x47554244` sibling),
//           allocates a child resource node via Resource_CreateNode (0x004B59F0), and splices `&child[2]`
//           into the parent container's child list (`arg2 + 0x14`) via HeapList_Insert (0x004AF8F0).
//           Calls Error_Show("Error in resource file: '%s'", context) on malformed chunk tags.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B60F0-0x004B637F, 655 bytes,
//                 digest 115d8cda2c658da73d9407d22baf6b6ee0562634c1e8d259034645ca75867cc6;
//                 CLEAN/sys/004B60F0-Sys_ErrorInResourceFileS.c.
// CFG: radare2 28 blocks/45 edges; Binary Ninja 28 blocks; 100% structural agreement.
// Confidence: high for exact FourCC constants (`NAME`, `BODY`, `IRES`, `CSTR`, `PATH`, `DBUG`) and child-list linking.
// Signature: x86 cdecl int32_t(const char* context, void* arg2, void* arg3, const int32_t* arg4).
// Usage/tests: invoked by Resource_RegisterBuffer (0x004B5F80) for `ILFF`/`IRES` containers; exercised by sys_resource_test.cpp.
std::int32_t Resource_RegisterIlffContainer(const char* context, void* arg2, void* arg3,
                                            const std::int32_t* arg4);
inline std::int32_t Sys_ErrorInResourceFileS(const char* context, void* arg2, void* arg3,
                                            const std::int32_t* arg4)
{
    return Resource_RegisterIlffContainer(context, arg2, arg3, arg4);
}

// Native: Resource_RegisterBuffer() - VA 0x004B5F80 - complete port.
// Behavior: registers a loaded buffer `arg2` of byte length `arg3` under `context`:
//           1. Calls Sys_AllocateMemory_5(&item_8, context) (0x004B1820).
//           2. If `arg2` is an `ILFF`/`IRES` container (`0x46464C49` / `0x53455249`), creates parent node
//              via Resource_CreateNode(context, nullptr, arg2, 0, 0, &item_8), populates children via
//              Resource_RegisterIlffContainer(context, data_2, arg2, &item_8), sets `data_2->loaded_flag = 1`,
//              and returns `arg2`.
//           3. Otherwise, if `arg3 < 8` or `Resource_MatchPrefix(arg2, "RESOURCE", 8) != 0`, creates
//              a standalone node (`kind = 2`, `size = arg3`) with `loaded_flag = 1`.
//           4. Otherwise (`"RESOURCE"` packed archive header), creates a parent node (`kind = 0`), sets
//              `loaded_flag = 1`, and iterates `arg2[2]` (`count`) entries starting at `&arg2[8]` (`+0x20`),
//              copying each entry name (`*(entry + 8)` bytes at `entry + 0x0C`), aligning payload start by
//              the retail `and al, 0xE0` name stride (low byte of `name_len + 0x2B`, bits 0-4 cleared,
//              upper bits preserved) and next entry by `(data_size + 0x1F) & 0xFFFFFFE0`, and
//              splicing each child (`kind = 1`) into `parent + 0x14`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B5F80-0x004B60E1, 353 bytes,
//                 digest 7008df670ec82806c208a728e050b93acf7ce0d8b4a333b9ef0d2ef74bae3442;
//                 CLEAN/sys/004B5F80-Sys_Resource.c.
// CFG: radare2 11 blocks/16 edges; Binary Ninja 11 blocks; 94.4% structural agreement.
// Confidence: high for all three container branches (`ILFF`/`IRES`, `"RESOURCE"`, and raw file).
// Signature: x86 cdecl void*(const char* context, void* arg2, int32_t arg3).
// Usage/tests: invoked by Resource_Load (0x004B5F00) after unpacking a resource file; exercised by sys_resource_test.cpp.
void* Resource_RegisterBuffer(const char* context, void* arg2, std::int32_t arg3);
inline void* Sys_Resource(const char* context, void* arg2, std::int32_t arg3)
{
    return Resource_RegisterBuffer(context, arg2, arg3);
}

// Host/test configuration for the resource node pool (`0x00943E20..0x00943E38`).
void Resource_InitPoolForTest(std::size_t capacity = 64);
[[nodiscard]] std::int32_t Resource_GetActiveCount() noexcept;
void Sys_SetAllocateMemory5(SysAllocateMemory5Fn fn) noexcept;
void Sys_SetOpenFileStream5(SysOpenFileStream5Fn fn) noexcept;

} // namespace igi::retail::resource
