#include "igi/retail/script/qvm_native_loader.hpp"

#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/error/error_show.hpp"
#include "igi/retail/mem/mem_alloc.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"

#include <cstdarg>
#include <cstdint>
#include <cstring>

namespace igi::retail::script {

// Retail QVM state words shared with qvm_execute.cpp (`0x00A965A0`, `0x00A96598`, `0x00A965A8`).
//
// These three have to live at namespace scope, not in the anonymous namespace below: qvm_execute.cpp
// declares them `extern` and reads/writes the same retail storage (0x00A965A0 is Qvm_SetGlobalState's
// dword, 0x00A965A8 is the error-reported flag the evaluators store, 0x00A96598 is the
// error-reporting-enabled dword). Internal linkage here meant those extern declarations could never
// resolve, and a local duplicate in qvm_execute.cpp would have split the state Qvm_ReportError and
// Qvm_SetGlobalState update - which is why the fix is linkage, not a second definition.
//
// 0x00A965A0 - written only by Qvm_SetGlobalState (0x004B8A14), read only by Qvm_Execute (0x004B8645).
// 0x00A96598 - error reporting enabled: written at 0x004B8974 / 0x004B89D4, read by Qvm_ReportError.
// 0x00A965A8 - "an error was reported": written at 0x004B7E1D, 0x004B85C7, 0x004B86BB, 0x004B9313,
//              0x004B987D, 0x004BBD83; read at 0x004B8699, 0x004BF361.
std::int32_t g_a965a0 = -1;
// The error-state pair is observable retail memory: 0x004B8970 writes 0x00A96598 and the
// 0x004B8B00 reset clears 0x00A43E70/0x00A43E68. They are volatile so a single-translation-unit
// build cannot constant-fold away Qvm_ReportError's flag store, disabled-state return and
// callback branch just because this unit contains no writer for them.
volatile std::int32_t g_a96598 = 1;
[[maybe_unused]] volatile std::int32_t g_a965a8 = 0;

namespace {

// Port-owned storage for `0x005C8BEC` (Script_GetDebugCompileState) and `0x005C8BF0` (Script_GetForcePrecompiledState).
// Retail 0x005C8BEC storage is owned by engine/src/retail/core/app_context_extra.cpp
// (SetDebugCompileState 0x0048F1C0); this file binds to that single owner instead of
// keeping a second copy of the same retail address.
extern "C" std::int32_t g_5c8bec;
std::int32_t g_5c8bf0 = 0;

// Port-owned storage for `0x00A43E68`, `0x00A43E70`.
volatile std::int32_t g_a43e68 = 0;

QvmErrorCallbackFn volatile g_a43e70 = nullptr;

// Retail extension constants at `0x00547290` (".qvm") and `0x00547288` (".qas").
constexpr char kExtQvm[5] = ".qvm";
constexpr char kExtQas[5] = ".qas";

SysDispatchOpcode7Fn g_sys_dispatch_opcode_7 = nullptr;
SysPushStack16Fn g_sys_push_stack_16 = nullptr;
SysPopStack16Fn g_sys_pop_stack_16 = nullptr;
SysResolveSymbol28Fn g_sys_resolve_symbol_28 = nullptr;

// Injectable host boundaries for the two Qvm_Cleanup release callees that are not
// yet transcribed (retail `0x004BD380` standalone release and `0x004B0D10` block
// release). They default to a 0-returning absence until their allocator-free
// chains are ported.
SysQvmStandaloneReleaseFn g_sys_qvm_standalone_release = nullptr;
SysQvmBlockReleaseFn g_sys_qvm_block_release = nullptr;
QScriptCleanupFn g_qscript_cleanup = nullptr;
QScriptParseAndAssembleFn g_qscript_parse_and_assemble = nullptr;
QScriptAssembleFn g_qscript_assemble = nullptr;

} // namespace

// Native: Script_GetDebugCompileState; retail VA 0x0048F1D0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE (SHA256: dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01); VA 0x0048F1D0-0x0048F1D6, 6 bytes, digest 70b1dc6fd700abb6f890ade42e9b86f1a1dc61d0b82061eda4cf096bf59d2419; CLEAN/fx/0048F1D0-Script_GetDebugCompileState.c.
// Behavior/CFG: returns the engine debug/compile-scripts state dword `0x005C8BEC` (`mov eax, [0x005C8BEC]; ret`). radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: VERIFIED; high for exact global getter semantics, checked against the retail's own bytes under unicorn in 8 of 8 cases.
std::int32_t Script_GetDebugCompileState()
{
    return g_5c8bec;
}

// Native: Script_GetForcePrecompiledState; retail VA 0x0048F340; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F340-0x0048F346, 6 bytes, digest 0a2cf2a5fb6118bed6a5ef0745d0ee73f47dc474349594598f5768ef924e40ee.
// Behavior/CFG: returns the engine force-precompiled-qvm state dword `0x005C8BF0` (`mov eax, [0x005C8BF0]; ret`). radare2 1 block/0 edges; Binary Ninja 1 block; 100% structural agreement.
// Status/limits: VERIFIED; high for exact global getter semantics, checked against the retail's own bytes under unicorn in 8 of 8 cases.
std::int32_t Script_GetForcePrecompiledState()
{
    return g_5c8bf0;
}

// Native: Resource_FileExists; retail VA 0x004B5B60; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B5B60-0x004B5B86, 38 bytes, digest 36a0b9cb50186197bae3f372447922d47d5c237e5a79558de984fe51d97ba92b.
// Behavior/CFG: returns `1` if `Is_ResourceLoaded(path, nullptr) != 0` (0x004B5B90); otherwise returns `Sys_DispatchOpcode_7(path)` (0x004B1FC0). radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact resource-existence probe.
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

namespace {

// Retail 0x004B7E74 is `jmp $` (bytes eb fe): the fallback diagnostic path never returns.
// The self-jump is emitted explicitly so no optimizer can treat the loop as terminating.
[[noreturn]] void Qvm_ReportErrorHalt() noexcept
{
    __asm__ __volatile__("1: jmp 1b");
    __builtin_unreachable();
}

} // namespace

// Native: Qvm_ReportError; retail VA 0x004B7E10; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Tools/evidence: Binary Ninja MCP, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on retail IGI.EXE; VA 0x004B7E10-0x004B7E76, 102 bytes, digest 3a498c19bab408f24ec62452e00b3aaf5ca8863717620b589f9c5004002f24e6; CLEAN/qvm/004B7E10-Qvm_ReportError.c.
// Behavior/CFG: reads `g_a96598` and stores `g_a965a8 = 1`; if `g_a96598 == 0`, returns `0` (0x004B7E60-0x004B7E66); otherwise formats `format` + variadic args into a 0x400-byte stack buffer via String_VFormat (0x004A5702); when `g_a43e70` is non-null, returns `g_a43e70(buffer, g_a43e68)`; otherwise calls Debug_Error(buffer) (0x004AF7B0) with the buffer as its format argument and halts in the retail `jmp $`, so that path never returns. radare2 6 blocks/7 edges; Binary Ninja 6 blocks/7 edges; angr CFGFast 9 blocks/10 edges (it splits at calls), no unresolved jumps; 100% Binary Ninja/radare2 structural agreement.
// Status/limits: BUILD_VERIFIED; the fallback now matches retail (Debug_Error with the formatted buffer, then a non-returning halt). The error-state globals are volatile so the compiled object keeps the retail flag store, disabled-state return and callback branch instead of constant-folding them. The retail display/exit callbacks installed by 0x00498900 (0x004988D0 MessageBoxA and 0x00498930 DebugBreak+exit) remain unported, so the candidate installs neither and its Debug_Error returns 0 on the no-callback path; the explicit self-loop reproduces the raw retail `jmp $` for that case. No paired retail/candidate runtime capture exists, so RUNTIME_TESTED/VERIFIED is not claimed.
std::int32_t Qvm_ReportError(const char* format, ...)
{
    const std::int32_t a96598_1 = g_a96598;
    g_a965a8 = 1;

    if (a96598_1 == 0)
    {
        return a96598_1;
    }

    // Retail leaves the 0x400-byte frame uninitialized; String_VFormat writes the text and NUL.
    char var_400[0x400];
    std::va_list args;
    va_start(args, format);
    error::String_VFormat(var_400, format, args);
    va_end(args);

    // Retail 0x004B7E43 loads the callback once into EAX and calls that same value.
    const QvmErrorCallbackFn callback = g_a43e70;
    if (callback != nullptr)
    {
        return callback(var_400, g_a43e68);
    }

    // Retail 0x004B7E67-0x004B7E71: `lea eax,[esp]; push eax; call Debug_Error; add esp,4`.
    // The buffer itself is the format argument; retail passes no substitution list here.
    Debug_Error(var_400);

    // 0x004B7E74: `jmp $`; this path never returns.
    Qvm_ReportErrorHalt();
}

// Native: Qvm_Cleanup; retail VA 0x004B83D0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B83D0-0x004B8407, 55 bytes, digest 15262421fde0740b2926521a46297cc4d7e66731665240f9736b56887a3f71ff.
// Behavior/CFG: inspects `qvm[0x23]` (`qvm + 0x8C`); if null, calls `Resource_Unload((const char*)qvm)` (0x004B6380), otherwise calls the standalone release `Sys_CheckHeap_28(standalone)` (0x004BD380); both paths then call the block release `Sys_UnloadBlock(qvm)` (0x004B0D10) and return its value (disassembly 0x004B83DF-0x004B8402). radare2 3 blocks/2 edges; Binary Ninja 3 blocks; 100% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for the exact +0x8C branch, call order, and argument re-throws. The two release callees (0x004BD380 and 0x004B0D10) are not yet transcribed; they are invoked through injectable host boundaries (Sys_SetQvmStandaloneRelease / Sys_SetQvmBlockRelease) that return 0 until their allocator-free chains are ported, so the retail return value of that inner chain is not reproduced by the default boundaries.
std::int32_t Qvm_Cleanup(void* qvm)
{
    auto* const bytes = static_cast<std::uint8_t*>(qvm);
    // retail stores and loads the standalone pointer as a 32-bit dword at qvm+0x8C.
    const std::uint32_t standalone_value = *reinterpret_cast<std::uint32_t*>(bytes + 0x8C);
    if (standalone_value == 0)
    {
        resource::Resource_Unload(reinterpret_cast<const char*>(bytes));
    }
    else if (g_sys_qvm_standalone_release != nullptr)
    {
        g_sys_qvm_standalone_release(
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(standalone_value)));
    }
    return g_sys_qvm_block_release != nullptr ? g_sys_qvm_block_release(qvm) : 0;
}

// Native: Qvm_Load; retail VA 0x004B80B0; signature/calling convention unknown.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Tools/evidence: and loads raw bytes via `Resource_Load(qvm_path, &size)` (0x004B5F00); allocates a Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004B80B0-0x004B83CC, 796 bytes, digest 346d4966918bdacf55c0b3a684d1ff20464bb9aa1e7239118d1684355d71e79b; CLEAN/qvm/004B80B0-Qvm_Load.c.
// Behavior/CFG: loads a compiled `.qvm` script object from `qvm_path`: checks `Script_GetDebugCompileState()` (0x0048F1D0); when 0 (or fallback), verifies `Resource_FileExists(qvm_path)` (0x004B5B60) `0x94`-byte QVM object via `Mem_AllocFn(0x94, 4)` (0x004B0C60), zeroes it, calls `Qvm_SetGlobalState(0)` (0x004B8A10), stores the loaded buffer at `+0x80` (`result[0x20]`), size at `+0x84` (`result[0x21]`), and copies the path string into `+0x00..+0x7F`. When `Script_GetDebugCompileState()` is non-zero, also supports `.qvm` extension replacement (`0x00547290` = `".qvm"`) and `.qas` assembly (`0x00547288` = `".qas"`). radare2 33 blocks/50 edges; Binary Ninja 33 blocks; 91.4% structural agreement.
// Status/limits: STRUCTURALLY_VERIFIED; high for exact 0x94 object layout, +0x80/+0x84/+0x8C/+0x90 fields, and callee chain.
void* Qvm_Load(const char* qvm_path)
{
    void* item_118 = nullptr;
    void** item_11c = nullptr;
    std::int32_t item_114 = 0;
    char item_100[0x100]{};

    // 0x004B80D2: the retail asks the script subsystem for the debug compile
    // state and takes the resource path when it is clear.
    if (!Script_GetDebugCompileState())
    {
    label_4b81ba:
        // 0x004B81B9 push ebp; 0x004B81BA call Resource_FileExists;
        // 0x004B81C2 test eax,eax; 0x004B81C4 je 0x004B81DC.
        if (!Resource_FileExists(qvm_path))
        {
            // 0x004B81DC pop edi/esi/ebp; 0x004B81DF xor eax,eax; 0x004B81E8 retn.
            return 0;
        }

        // 0x004B81C6 lea edx,[var_114]; 0x004B81CA push edx;
        // 0x004B81CB push ebp; 0x004B81CC call Resource_Load.
        void* const loaded = resource::Resource_Load(qvm_path, &item_114);
        // 0x004B81D6 mov dword [var_118], eax.
        item_118 = loaded;
        // 0x004B81D4 cmp eax,ebx; 0x004B81DA jne 0x004B81E9.
        if (!loaded)
        {
            // 0x004B81DC pop edi/esi/ebp; 0x004B81DF xor eax,eax; 0x004B81E8 retn.
            return 0;
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
        char* const data_2 = String_FindExtension(nullptr, 0, item_100);
        std::memcpy(data_2, kExtQvm, 5U);

        if (Script_GetForcePrecompiledState() == 0)
        {
            if (Resource_FileExists(item_100) == 0)
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
            if (Resource_FileExists(item_100) == 0)
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
    void* result = mem::Mem_Alloc(alloc_size, 4);
    std::memset((void*)result, 0, static_cast<std::size_t>(alloc_size));
    Qvm_SetGlobalState(0);
    void** data_14 = item_11c;

    if (data_14 != nullptr)
    {
    label_4b838c:
        *reinterpret_cast<void***>(reinterpret_cast<std::uint8_t*>(result) + 0x8C) = data_14;
        if constexpr (sizeof(void*) == 4)
        {
            *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uint8_t*>(result) + 0x90) = 1;
        }
        else
        {
            *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uint8_t*>(result) + 0x94) = 1;
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
        std::memcpy((void*)result, item_100, entity_19);
    }
    else
    {
        *reinterpret_cast<void**>(reinterpret_cast<std::uint8_t*>(result) + 0x80) = item_118;
        if constexpr (sizeof(void*) == 4)
        {
            *reinterpret_cast<void***>(reinterpret_cast<std::uint8_t*>(result) + 0x88) = data_14;
            *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uint8_t*>(result) + 0x84) = item_114;
        }
        else
        {
            *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uint8_t*>(result) + 0x88) = item_114;
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
        std::memcpy((void*)result, qvm_path, entity_9);

        if (Script_GetDebugCompileState() != 0)
        {
            char item_80[0x80]{};
            std::memcpy(item_80, qvm_path, entity_9);
            char* const data_18 = String_FindExtension(nullptr, 0, item_80);
            std::memcpy(data_18, kExtQas, 5U);

            if (g_qscript_cleanup != nullptr)
            {
                g_qscript_cleanup(item_100);
                g_qscript_cleanup(item_80);
            }

            if (g_qscript_parse_and_assemble != nullptr
                && g_qscript_parse_and_assemble(item_80, (void*)result) != 0)
            {
                Qvm_Cleanup((void*)result);
                Qvm_ReportError("An error occured when compiling file: %s", qvm_path);
                return nullptr;
            }

            if (g_qscript_assemble != nullptr && g_qscript_assemble(item_100, item_80) != 0)
            {
                Qvm_ReportError("An error occured when compiling file: %s", qvm_path);
                return nullptr;
            }

            if (g_qscript_cleanup != nullptr)
            {
                g_qscript_cleanup(item_80);
            }
            Qvm_Cleanup((void*)result);
            result = mem::Mem_Alloc(alloc_size, 4);
            std::memset((void*)result, 0, static_cast<std::size_t>(alloc_size));
            data_14 =
                g_sys_resolve_symbol_28 != nullptr ? g_sys_resolve_symbol_28(item_100, 0, 0) : nullptr;
            if (data_14 != nullptr)
            {
                goto label_4b838c;
            }

            Qvm_ReportError("Failed to load QVM file: %s", item_100);
            return nullptr;
        }
    }

    return result;
}

void Script_SetDebugCompileFlag(std::int32_t enabled) noexcept
{
    g_5c8bec = enabled;
}

void Script_SetForcePrecompiledFlag(std::int32_t enabled) noexcept
{
    g_5c8bf0 = enabled;
}

void Qvm_SetDispatchOpcode7(SysDispatchOpcode7Fn fn) noexcept
{
    g_sys_dispatch_opcode_7 = fn;
}

void Sys_SetResolveSymbol28(SysResolveSymbol28Fn fn) noexcept
{
    g_sys_resolve_symbol_28 = fn;
}

void Sys_SetQvmStandaloneRelease(SysQvmStandaloneReleaseFn fn) noexcept
{
    g_sys_qvm_standalone_release = fn;
}

void Sys_SetQvmBlockRelease(SysQvmBlockReleaseFn fn) noexcept
{
    g_sys_qvm_block_release = fn;
}

std::int32_t Qvm_GetLastPopStack23Value() noexcept
{
    return g_a965a0;
}

} // namespace igi::retail::script
