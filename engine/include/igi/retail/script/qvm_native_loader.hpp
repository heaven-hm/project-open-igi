#pragma once

#include <cstdint>

namespace igi::retail::script {

using SysDispatchOpcode7Fn = std::int32_t (*)(const char* path);
using SysPushStack16Fn = void (*)(void* dst_stat, const char* path);
using SysPopStack16Fn = std::int32_t (*)(const void* left_stat, const void* right_stat);
using SysResolveSymbol28Fn = void** (*)(const char* path, std::int32_t arg2, std::int32_t arg3);
using QScriptCleanupFn = void (*)(const char* path);
using QScriptParseAndAssembleFn = std::int32_t (*)(const char* qas_path, void* qvm_obj);
using QScriptAssembleFn = std::int32_t (*)(const char* qvm_path, const char* qas_path);
using QvmErrorCallbackFn = std::int32_t (*)(const char* message, std::int32_t user_data);

// Injectable boundaries for the two Qvm_Cleanup release callees that are not yet
// transcribed: retail `0x004BD380` (standalone block release) and `0x004B0D10`
// (pointer-header block release). The defaults return 0 until their allocator-free
// chains are ported.
using SysQvmStandaloneReleaseFn = std::int32_t (*)(void* standalone);
using SysQvmBlockReleaseFn = std::int32_t (*)(void* context);

// Native: Script_GetDebugCompileState() - VA 0x0048F1D0 - complete port.
// Previous catalog alias: FX_FragmentSimulate_24.
// Behavior: returns the engine debug/compile-scripts state dword `0x005C8BEC` (`mov eax, [0x005C8BEC]; ret`).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01);
//                 VA 0x0048F1D0-0x0048F1D6, 6 bytes, digest 70b1dc6fd700abb6f890ade42e9b86f1a1dc61d0b82061eda4cf096bf59d2419;
//                 CLEAN/fx/0048F1D0-FX_FragmentSimulate_24.c.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact global getter semantics.
// Signature: x86 cdecl int32_t(void); ported as std::int32_t Script_GetDebugCompileState().
// Usage/tests: queried by Qvm_Load to select direct resource load vs .qvm/.qas compile path; exercised by qvm_native_loader_test.cpp.
[[nodiscard]] std::int32_t Script_GetDebugCompileState();
inline std::int32_t FX_FragmentSimulate_24() { return Script_GetDebugCompileState(); }

// Native: Script_GetForcePrecompiledState() - VA 0x0048F340 - complete port.
// Previous catalog alias: FX_FireBallUpdate_27.
// Behavior: returns the engine force-precompiled-qvm state dword `0x005C8BF0` (`mov eax, [0x005C8BF0]; ret`).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F340-0x0048F346, 6 bytes,
//                 digest 0a2cf2a5fb6118bed6a5ef0745d0ee73f47dc474349594598f5768ef924e40ee.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact global getter semantics.
// Signature: x86 cdecl int32_t(void); ported as std::int32_t Script_GetForcePrecompiledState().
// Usage/tests: queried by Qvm_Load when Script_GetDebugCompileState() is non-zero; exercised by qvm_native_loader_test.cpp.
[[nodiscard]] std::int32_t Script_GetForcePrecompiledState();
inline std::int32_t FX_FireBallUpdate_27() { return Script_GetForcePrecompiledState(); }

// Native: Resource_FileExists() - VA 0x004B5B60 - complete port.
// Previous catalog alias: Sys_AllocateMemory_16.
// Behavior: returns `1` if `Is_ResourceLoaded(path, nullptr) != 0` (0x004B5B90); otherwise
//           returns `Sys_DispatchOpcode_7(path)` (0x004B1FC0).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B5B60-0x004B5B86, 38 bytes,
//                 digest 36a0b9cb50186197bae3f372447922d47d5c237e5a79558de984fe51d97ba92b.
// CFG: radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Confidence: high for exact resource-existence probe.
// Signature: x86 cdecl int32_t(const char* path); ported as std::int32_t Resource_FileExists(const char* path).
// Usage/tests: probes whether a script/QVM resource exists before loading; exercised by qvm_native_loader_test.cpp.
std::int32_t Resource_FileExists(const char* path);
inline std::int32_t Sys_AllocateMemory_16(const char* path) { return Resource_FileExists(path); }

// Native: Qvm_SetGlobalState() - VA 0x004B8A10 - complete port.
// Previous catalog alias: Sys_PopStack_23.
// Behavior: writes `arg1` to `0x00A965A0` and returns `arg1`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B8A10-0x004B8A1A, 10 bytes,
//                 digest d28479bf213d0e99659b2abd6b1af2f9bed47507dc5682bd49efcc6f6a0c38de.
// CFG: radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Confidence: high for exact global store.
// Signature: x86 cdecl int32_t(int32_t arg1); ported as std::int32_t Qvm_SetGlobalState(std::int32_t arg1).
// Usage/tests: resets QVM state global 0x00A965A0 during Qvm_Load; exercised by qvm_native_loader_test.cpp.
std::int32_t Qvm_SetGlobalState(std::int32_t arg1);
inline std::int32_t Sys_PopStack_23(std::int32_t arg1) { return Qvm_SetGlobalState(arg1); }

// Native: String_FindExtension() - VA 0x004B1E90 - complete port.
// Previous catalog alias: Sys_OpenFileStream_7.
// Behavior: scans `path` (`arg3`) backwards from its NUL terminator to locate the last `'.'`
//           (`0x2E`) character (or `path + 1` if none), optionally copies up to `arg2 - 1`
//           characters into `dst` (`arg1`) when `dst != nullptr`, and returns a pointer to the
//           extension within `path`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B1E90-0x004B1EDB, 75 bytes,
//                 digest c3f81afb409c78761c6da27aed3170b4c127b7dece51e50c4c0b0be10684edc7.
// CFG: radare2 6 blocks/8 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Confidence: high for exact reverse extension scan.
// Signature: x86 cdecl char*(char* dst, int32_t max_len, char* path).
// Usage/tests: locates file extension in Qvm_Load so `.qvm` or `.qas` can be written in-place; exercised by qvm_native_loader_test.cpp.
char* String_FindExtension(char* dst, std::int32_t max_len, char* path);
inline char* Sys_OpenFileStream_7(char* dst, std::int32_t max_len, char* path)
{
    return String_FindExtension(dst, max_len, path);
}

// Native: Qvm_ReportError() - VA 0x004B7E10 - complete port.
// Previous catalog alias: Sys_FreeMemory_22.
// Behavior: sets `g_a965a8 = 1`; if `g_a96598 == 0`, returns `0`; otherwise formats `format` +
//           variadic args into a 0x400-byte buffer via String_VFormat (0x004A5702) and either
//           invokes `g_a43e70(buf, g_a43e68)` when non-null or calls Error_Show(buf) (0x004AF7B0).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B7E10-0x004B7E76, 102 bytes,
//                 digest 3a498c19bab408f24ec62452e00b3aaf5ca8863717620b589f9c5004002f24e6.
// CFG: radare2 6 blocks/6 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Confidence: high for exact error-flag store, callback branch, and Error_Show fallback.
// Signature: x86 cdecl int32_t(const char* format, ...); ported as variadic function.
// Usage/tests: QVM compilation/load error reporter called by Qvm_Load; exercised by qvm_native_loader_test.cpp.
std::int32_t Qvm_ReportError(const char* format, ...);
template <typename... Args>
inline std::int32_t Sys_FreeMemory_22(const char* format, Args&&... args)
{
    return Qvm_ReportError(format, static_cast<Args&&>(args)...);
}

// Native: Qvm_Cleanup() - VA 0x004B83D0 - complete port.
// Behavior: inspects `qvm[0x23]` (`qvm + 0x8C`); if null, calls `Resource_Unload((const char*)qvm)`
//           (0x004B6380) and `Sys_ExecuteBytecode_2(qvm)` (0x004B0D10); otherwise calls
//           `Sys_CheckHeap_28(qvm[0x23])` (0x004BD380) and `Sys_ExecuteBytecode_2(qvm)`.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B83D0-0x004B8407, 55 bytes,
//                 digest 15262421fde0740b2926521a46297cc4d7e66731665240f9736b56887a3f71ff.
// CFG: radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Confidence: high for exact +0x8C branch and Resource_Unload / heap free sequence.
// Signature: x86 cdecl int32_t(void* qvm); ported as std::int32_t Qvm_Cleanup(void* qvm).
// Usage/tests: releases loaded QVM script objects; exercised by qvm_native_loader_test.cpp.
std::int32_t Qvm_Cleanup(void* qvm);

// Native: Qvm_Load() - VA 0x004B80B0 - complete port.
// Behavior: loads a compiled `.qvm` script object from `qvm_path`: checks `FX_FragmentSimulate_24()`
//           (0x0048F1D0); when 0 (or fallback), verifies `Sys_AllocateMemory_16(qvm_path)` (0x004B5B60)
//           and loads raw bytes via `Resource_Load(qvm_path, &size)` (0x004B5F00); allocates a
//           `0x94`-byte QVM object via `Mem_Alloc(0x94, 4)` (0x004B0C60), zeroes it, calls
//           `Sys_PopStack_23(0)` (0x004B8A10), stores the loaded buffer at `+0x80` (`result[0x20]`),
//           size at `+0x84` (`result[0x21]`), and copies the path string into `+0x00..+0x7F`.
//           When `FX_FragmentSimulate_24()` is non-zero, also supports `.qvm` extension replacement
//           (`0x00547290` = `".qvm"`) and `.qas` assembly (`0x00547288` = `".qas"`).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B80B0-0x004B83CC, 796 bytes,
//                 digest 346d4966918bdacf55c0b3a684d1ff20464bb9aa1e7239118d1684355d71e79b;
//                 CLEAN/qvm/004B80B0-Qvm_Load.c.
// CFG: radare2 33 blocks/50 edges; Binary Ninja 33 blocks; 91.4% structural agreement.
// Confidence: high for exact 0x94 object layout, +0x80/+0x84/+0x8C/+0x90 fields, and callee chain.
// Signature: x86 cdecl void*(const char* qvm_path); ported as void* Qvm_Load(const char* qvm_path).
// Usage/tests: retail QVM script loader; exercised by qvm_native_loader_test.cpp.
void* Qvm_Load(const char* qvm_path);

// Host/test configuration for QVM globals and optional compiler callbacks.
void Script_SetDebugCompileFlag(std::int32_t enabled) noexcept;
void Script_SetForcePrecompiledFlag(std::int32_t enabled) noexcept;
void Qvm_SetDispatchOpcode7(SysDispatchOpcode7Fn fn) noexcept;
void Sys_SetResolveSymbol28(SysResolveSymbol28Fn fn) noexcept;
void Sys_SetQvmStandaloneRelease(SysQvmStandaloneReleaseFn fn) noexcept;
void Sys_SetQvmBlockRelease(SysQvmBlockReleaseFn fn) noexcept;
[[nodiscard]] std::int32_t Qvm_GetLastPopStack23Value() noexcept;

} // namespace igi::retail::script
