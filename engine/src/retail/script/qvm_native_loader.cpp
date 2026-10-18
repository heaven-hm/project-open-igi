#include "igi/retail/script/qvm_native_loader.hpp"

#include "igi/retail/error/error_show.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"

#include <cstdarg>
#include <cstring>

namespace igi::retail::script {
namespace {

// Port-owned storage for `0x005C8BEC` (FX_FragmentSimulate_24) and `0x005C8BF0` (FX_FireBallUpdate_27).
std::int32_t g_5c8bec = 0;
std::int32_t g_5c8bf0 = 0;

// Port-owned storage for `0x00A965A0` (Sys_PopStack_23), `0x00A96598`, `0x00A965A8`, `0x00A43E68`, `0x00A43E70`.
std::int32_t g_a965a0 = -1;
std::int32_t g_a96598 = 1;
[[maybe_unused]] std::int32_t g_a965a8 = 0;
std::int32_t g_a43e68 = 0;

QvmErrorCallbackFn g_a43e70 = nullptr;

// Retail extension constants at `0x00547290` (".qvm") and `0x00547288` (".qas").
constexpr char kExtQvm[5] = ".qvm";
constexpr char kExtQas[5] = ".qas";

SysDispatchOpcode7Fn g_sys_dispatch_opcode_7 = nullptr;
SysPushStack16Fn g_sys_push_stack_16 = nullptr;
SysPopStack16Fn g_sys_pop_stack_16 = nullptr;
SysResolveSymbol28Fn g_sys_resolve_symbol_28 = nullptr;
QScriptCleanupFn g_qscript_cleanup = nullptr;
QScriptParseAndAssembleFn g_qscript_parse_and_assemble = nullptr;
QScriptAssembleFn g_qscript_assemble = nullptr;

} // namespace

// Native: Script_GetDebugCompileState; retail VA 0x0048F1D0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x0048F1D0-0x0048F1D6, 6 bytes, digest 70b1dc6fd700abb6f890ade42e9b86f1a1dc61d0b82061eda4cf096bf59d2419; CLEAN/fx/0048F1D0-FX_FragmentSimulate_24.c.
// Behavior/CFG: returns the engine debug/compile-scripts state dword `0x005C8BEC` (`mov eax, [0x005C8BEC]; ret`). radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact global getter semantics.
// Previous catalog alias: FX_FragmentSimulate_24.
std::int32_t Script_GetDebugCompileState()
{
    return g_5c8bec;
}

// Native: Script_GetForcePrecompiledState; retail VA 0x0048F340; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F340-0x0048F346, 6 bytes, digest 0a2cf2a5fb6118bed6a5ef0745d0ee73f47dc474349594598f5768ef924e40ee.
// Behavior/CFG: returns the engine force-precompiled-qvm state dword `0x005C8BF0` (`mov eax, [0x005C8BF0]; ret`). radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact global getter semantics.
// Previous catalog alias: FX_FireBallUpdate_27.
std::int32_t Script_GetForcePrecompiledState()
{
    return g_5c8bf0;
}

// Native: Resource_FileExists; retail VA 0x004B5B60; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B5B60-0x004B5B86, 38 bytes, digest 36a0b9cb50186197bae3f372447922d47d5c237e5a79558de984fe51d97ba92b.
// Behavior/CFG: returns `1` if `Is_ResourceLoaded(path, nullptr) != 0` (0x004B5B90); otherwise returns `Sys_DispatchOpcode_7(path)` (0x004B1FC0). radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact resource-existence probe.
// Previous catalog alias: Sys_AllocateMemory_16.
std::int32_t Resource_FileExists(const char* path)
{
    if (resource::Is_ResourceLoaded(path, nullptr) == 0)
    {
        return g_sys_dispatch_opcode_7 != nullptr ? g_sys_dispatch_opcode_7(path) : 0;
    }
    return 1;
}

// Native: Qvm_SetGlobalState; retail VA 0x004B8A10; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B8A10-0x004B8A1A, 10 bytes, digest d28479bf213d0e99659b2abd6b1af2f9bed47507dc5682bd49efcc6f6a0c38de.
// Behavior/CFG: writes `arg1` to `0x00A965A0` and returns `arg1`. radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact global store.
// Previous catalog alias: Sys_PopStack_23.
std::int32_t Qvm_SetGlobalState(std::int32_t arg1)
{
    g_a965a0 = arg1;
    return arg1;
}

// Native: String_FindExtension; retail VA 0x004B1E90; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B1E90-0x004B1EDB, 75 bytes, digest c3f81afb409c78761c6da27aed3170b4c127b7dece51e50c4c0b0be10684edc7.
// Behavior/CFG: scans `path` (`arg3`) backwards from its NUL terminator to locate the last `'.'` (`0x2E`) character (or `path + 1` if none), optionally copies up to `arg2 - 1` characters into `dst` (`arg1`) when `dst != nullptr`, and returns a pointer to the extension within `path`. radare2 6 blocks/8 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact reverse extension scan.
// Previous catalog alias: Sys_OpenFileStream_7.
char* String_FindExtension(char* dst, std::int32_t max_len, char* path)
{
    const char* edi = path;
    std::uint32_t i = 0xFFFFFFFFU;

    while (i != 0U)
    {
        const bool cond = (*edi != '\0');
        ++edi;
        --i;
        if (!cond)
        {
            break;
        }
    }

    const std::uint32_t ecx = ~i;
    std::uint32_t ecx_1 = ecx - 1U;

    if (ecx != 1U)
    {
        while (path[ecx_1] != '.')
        {
            const std::uint32_t temp1_1 = ecx_1;
            ecx_1 -= 1U;
            if (temp1_1 == 1U)
            {
                break;
            }
        }
    }

    char* const result = &path[ecx_1];
    if (dst == nullptr)
    {
        return result;
    }

    if (max_len > 1)
    {
        std::strncpy(dst, result, static_cast<std::size_t>(max_len - 1));
    }
    if (max_len > 0)
    {
        dst[max_len - 1] = '\0';
    }
    return result;
}

// Native: Qvm_ReportError; retail VA 0x004B7E10; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B7E10-0x004B7E76, 102 bytes, digest 3a498c19bab408f24ec62452e00b3aaf5ca8863717620b589f9c5004002f24e6.
// Behavior/CFG: sets `g_a965a8 = 1`; if `g_a96598 == 0`, returns `0`; otherwise formats `format` + variadic args into a 0x400-byte buffer via String_VFormat (0x004A5702) and either invokes `g_a43e70(buf, g_a43e68)` when non-null or calls Error_Show(buf) (0x004AF7B0). radare2 6 blocks/6 edges; Binary Ninja 6 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact error-flag store, callback branch, and Error_Show fallback.
// Previous catalog alias: Sys_FreeMemory_22.
std::int32_t Qvm_ReportError(const char* format, ...)
{
    const std::int32_t a96598_1 = g_a96598;
    g_a965a8 = 1;

    if (a96598_1 == 0)
    {
        return a96598_1;
    }

    char var_400[0x400]{};
    std::va_list args;
    va_start(args, format);
    error::String_VFormat(var_400, format, args);
    va_end(args);

    if (g_a43e70 != nullptr)
    {
        return g_a43e70(var_400, g_a43e68);
    }

    return error::Error_Show("%s", var_400);
}

// Native: Qvm_Cleanup; retail VA 0x004B83D0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B83D0-0x004B8407, 55 bytes, digest 15262421fde0740b2926521a46297cc4d7e66731665240f9736b56887a3f71ff.
// Behavior/CFG: inspects `qvm[0x23]` (`qvm + 0x8C`); if null, calls `Resource_Unload((const char*)qvm)` (0x004B6380); otherwise releases the standalone block. radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact +0x8C branch and Resource_Unload call.
std::int32_t Qvm_Cleanup(void* qvm)
{
    auto* const bytes = static_cast<std::uint8_t*>(qvm);
    void* const standalone = *reinterpret_cast<void**>(bytes + 0x8C);
    if (standalone == nullptr)
    {
        return resource::Resource_Unload(reinterpret_cast<const char*>(bytes));
    }
    return 0;
}

// Native: Qvm_Load; retail VA 0x004B80B0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: and loads raw bytes via `Resource_Load(qvm_path, &size)` (0x004B5F00); allocates a Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B80B0-0x004B83CC, 796 bytes, digest 346d4966918bdacf55c0b3a684d1ff20464bb9aa1e7239118d1684355d71e79b; CLEAN/qvm/004B80B0-Qvm_Load.c.
// Behavior/CFG: loads a compiled `.qvm` script object from `qvm_path`: checks `FX_FragmentSimulate_24()` (0x0048F1D0); when 0 (or fallback), verifies `Sys_AllocateMemory_16(qvm_path)` (0x004B5B60) `0x94`-byte QVM object via `Mem_Alloc(0x94, 4)` (0x004B0C60), zeroes it, calls `Sys_PopStack_23(0)` (0x004B8A10), stores the loaded buffer at `+0x80` (`result[0x20]`), size at `+0x84` (`result[0x21]`), and copies the path string into `+0x00..+0x7F`. When `FX_FragmentSimulate_24()` is non-zero, also supports `.qvm` extension replacement (`0x00547290` = `".qvm"`) and `.qas` assembly (`0x00547288` = `".qas"`). radare2 33 blocks/50 edges; Binary Ninja 33 blocks; 91.4% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact 0x94 object layout, +0x80/+0x84/+0x8C/+0x90 fields, and callee chain.
void* Qvm_Load(const char* qvm_path)
{
    void* item_118 = nullptr;
    void** item_11c = nullptr;
    std::int32_t item_114 = 0;
    char item_100[0x100]{};

    if (FX_FragmentSimulate_24() == 0)
    {
    label_4b81ba:
        if (Sys_AllocateMemory_16(qvm_path) == 0)
        {
            return nullptr;
        }

        void* const data_11 = resource::Resource_Load(qvm_path, &item_114);
        item_118 = data_11;
        if (data_11 == nullptr)
        {
            return nullptr;
        }
    }
    else
    {
        const char* dest_1 = qvm_path;
        std::uint32_t i = 0xFFFFFFFFU;
        while (i != 0U)
        {
            const bool flag_0_1 = (*dest_1 != '\0');
            ++dest_1;
            --i;
            if (!flag_0_1)
            {
                break;
            }
        }
        const std::size_t entity_1 = ~i;
        std::memcpy(item_100, qvm_path, entity_1);
        char* const data_2 = Sys_OpenFileStream_7(nullptr, 0, item_100);
        std::memcpy(data_2, kExtQvm, 5U);

        if (FX_FireBallUpdate_27() == 0)
        {
            if (Sys_AllocateMemory_16(item_100) == 0)
            {
                goto label_4b81ba;
            }

            std::uint64_t item_108 = 0;
            std::uint64_t item_110 = 0;
            if (g_sys_push_stack_16 != nullptr)
            {
                g_sys_push_stack_16(&item_108, item_100);
                g_sys_push_stack_16(&item_110, qvm_path);
            }
            if (g_sys_pop_stack_16 != nullptr && g_sys_pop_stack_16(&item_108, &item_110) < 0)
            {
                goto label_4b81ba;
            }

            void** const data_9 =
                g_sys_resolve_symbol_28 != nullptr ? g_sys_resolve_symbol_28(item_100, 0, 0) : nullptr;
            item_11c = data_9;
            if (data_9 == nullptr)
            {
                goto label_4b81ba;
            }
        }
        else
        {
            if (Sys_AllocateMemory_16(item_100) == 0)
            {
                return nullptr;
            }

            void** const data_5 =
                g_sys_resolve_symbol_28 != nullptr ? g_sys_resolve_symbol_28(item_100, 0, 0) : nullptr;
            item_11c = data_5;
            if (data_5 == nullptr)
            {
                return nullptr;
            }
        }
    }

    constexpr std::int32_t alloc_size = sizeof(void*) == 4 ? 0x94 : 0xA0;
    auto* result = static_cast<std::uint8_t*>(mem::Mem_Alloc(alloc_size, 4));
    std::memset(result, 0, static_cast<std::size_t>(alloc_size));
    Sys_PopStack_23(0);
    void** data_14 = item_11c;

    if (data_14 != nullptr)
    {
    label_4b838c:
        *reinterpret_cast<void***>(result + 0x8C) = data_14;
        if constexpr (sizeof(void*) == 4)
        {
            *reinterpret_cast<std::int32_t*>(result + 0x90) = 1;
        }
        else
        {
            *reinterpret_cast<std::int32_t*>(result + 0x94) = 1;
        }
        const char* dest_13 = item_100;
        std::uint32_t i_1 = 0xFFFFFFFFU;
        while (i_1 != 0U)
        {
            const bool flag_3_1 = (*dest_13 != '\0');
            ++dest_13;
            --i_1;
            if (!flag_3_1)
            {
                break;
            }
        }
        const std::size_t entity_19 = ~i_1;
        std::memcpy(result, item_100, entity_19);
    }
    else
    {
        *reinterpret_cast<void**>(result + 0x80) = item_118;
        if constexpr (sizeof(void*) == 4)
        {
            *reinterpret_cast<void***>(result + 0x88) = data_14;
            *reinterpret_cast<std::int32_t*>(result + 0x84) = item_114;
        }
        else
        {
            *reinterpret_cast<std::int32_t*>(result + 0x88) = item_114;
        }

        const char* dest_5 = qvm_path;
        std::uint32_t i_2 = 0xFFFFFFFFU;
        while (i_2 != 0U)
        {
            const bool flag_4_1 = (*dest_5 != '\0');
            ++dest_5;
            --i_2;
            if (!flag_4_1)
            {
                break;
            }
        }
        const std::size_t entity_9 = ~i_2;
        std::memcpy(result, qvm_path, entity_9);

        if (FX_FragmentSimulate_24() != 0)
        {
            char item_80[0x80]{};
            std::memcpy(item_80, qvm_path, entity_9);
            char* const data_18 = Sys_OpenFileStream_7(nullptr, 0, item_80);
            std::memcpy(data_18, kExtQas, 5U);

            if (g_qscript_cleanup != nullptr)
            {
                g_qscript_cleanup(item_100);
                g_qscript_cleanup(item_80);
            }

            if (g_qscript_parse_and_assemble != nullptr
                && g_qscript_parse_and_assemble(item_80, result) != 0)
            {
                Qvm_Cleanup(result);
                Sys_FreeMemory_22("An error occured when compiling file: %s", qvm_path);
                return nullptr;
            }

            if (g_qscript_assemble != nullptr && g_qscript_assemble(item_100, item_80) != 0)
            {
                Sys_FreeMemory_22("An error occured when compiling file: %s", qvm_path);
                return result;
            }

            if (g_qscript_cleanup != nullptr)
            {
                g_qscript_cleanup(item_80);
            }
            Qvm_Cleanup(result);
            result = static_cast<std::uint8_t*>(mem::Mem_Alloc(alloc_size, 4));
            std::memset(result, 0, static_cast<std::size_t>(alloc_size));
            data_14 =
                g_sys_resolve_symbol_28 != nullptr ? g_sys_resolve_symbol_28(item_100, 0, 0) : nullptr;
            if (data_14 != nullptr)
            {
                goto label_4b838c;
            }

            Sys_FreeMemory_22("Failed to load QVM file: %s", item_100);
            return nullptr;
        }
    }

    return result;
}

void Host_SetQvmDebugCompileFlag(std::int32_t enabled) noexcept
{
    g_5c8bec = enabled;
}

void Host_SetQvmForcePrecompiledFlag(std::int32_t enabled) noexcept
{
    g_5c8bf0 = enabled;
}

void Host_SetSysDispatchOpcode7(SysDispatchOpcode7Fn fn) noexcept
{
    g_sys_dispatch_opcode_7 = fn;
}

void Host_SetSysResolveSymbol28(SysResolveSymbol28Fn fn) noexcept
{
    g_sys_resolve_symbol_28 = fn;
}

std::int32_t Qvm_GetLastPopStack23Value() noexcept
{
    return g_a965a0;
}

} // namespace igi::retail::script
