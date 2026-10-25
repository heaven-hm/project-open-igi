// The retail QTaskList natives, reconstructed from igi.exe.
//
// Reconstructed natively from the original retail image:
//   QTaskList_Register  0x004C1790..0x004C17B9 (42 bytes)
//   QTaskList_Remove    0x004C17C0..0x004C17FD (62 bytes)
//   QTaskList_New       0x004C1800..0x004C1821 (34 bytes)
// Artifact: retail igi.exe SHA256
//   dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// The list header is {count at +0x00, capacity at +0x04, pointer slots at
// +0x08}; Register compares count against capacity with a signed `jl`; Remove
// decrements the count and copies slot [new count] over the hit slot; New
// allocates capacity * 4 + 8 bytes through Mem_AllocFn(0x004B0C60) with
// alignment 4 and then stores the capacity and clears the count. Both error
// branches call Debug_Error(0x004AF7B0) and then execute the retail's `jmp $`;
// the port keeps that self-loop because Debug_Error has a real return path
// before an exit callback is installed (retail_error.hpp).
//
// QTaskList_Delete (0x004C1830) is deliberately not reconstructed here: it
// calls Mem_DeAllocFn(0x004B0D10), whose body tail-calls 0x004A6A50
// (Camera_UpdateTransform_9, a ten-block heap-free routine) that is not yet
// reconstructed, so Delete cannot be offered as a native without that chain.

#include "igi/retail/core/qtask_list.hpp"

#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstddef>
#include <cstdint>

namespace {

// The exact retail message string at 0x00547EF4, read from the image.
constexpr char qtask_list_full_message[] = "QTaskList is full";
// The exact retail message string at 0x00547F08, read from the image. The
// retail spelling ("unrigistered") is preserved.
constexpr char qtask_list_remove_message[] =
    "Trying to remove unrigistered QTask from QTaskList";

[[nodiscard]] std::uint32_t* list_words(void* task_list) noexcept
{
    return static_cast<std::uint32_t*>(task_list);
}

[[nodiscard]] std::uint32_t task_word(void* task) noexcept
{
    // Retail 0x004C17B0 stores the second stack argument as a dword; the
    // candidate's task identity is a 32-bit pointer on the x86 target.
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(task));
}

} // namespace

extern "C" {

// Native: QTaskList_New; retail VA 0x004C1800
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x004C1800/capture.json, per-tool records docs/fidelity/evidence/0x004C1800-*.json
// Behavior/CFG: one block, no branches; `mov esi,[esp+4]; push 4; lea eax,[esi*4+8]; push eax; call Mem_AllocFn; add esp,8; mov [eax+4],esi; mov dword [eax],0; pop esi; retn`; returns the list pointer in EAX
// Status/limits: STRUCTURALLY_VERIFIED; CFG 1.000 (BN 1 block = r2 1 block) and angr exact entry; behavioural differential and paired runtime are pending
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
void* NATIVE_DECL QTaskList_New(std::int32_t capacity)
{
    void* const task_list =
        igi::retail::mem::Mem_Alloc((capacity * 4) + 8, 4);
    std::uint32_t* const words = list_words(task_list);
    words[1] = static_cast<std::uint32_t>(capacity); // +0x04: capacity
    words[0] = 0U;                                   // +0x00: count
    return task_list;
}

// Native: QTaskList_Register; retail VA 0x004C1790
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x004C1790/capture.json, per-tool records docs/fidelity/evidence/0x004C1790-*.json
// Behavior/CFG: four blocks; `eax=[esp+4]; ecx=[eax]; edx=[eax+4]; cmp ecx,edx; jl 0x4C17AC` (signed); full-list path pushes "QTaskList is full" (0x00547EF4), calls Debug_Error, `jmp $`; success path `edx=[esp+8]; mov [eax+ecx*4+8],edx; ecx=[eax]; inc ecx; mov [eax],ecx; retn`; returns the list pointer
// Status/limits: STRUCTURALLY_VERIFIED; CFG 1.000 (BN 4 blocks = r2 4 blocks) and angr exact entry; fatal path terminates in the retail self-loop until the caller path is runtime-observed
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
void* NATIVE_DECL QTaskList_Register(void* task_list, void* task)
{
    std::uint32_t* const words = list_words(task_list);
    const std::int32_t count = static_cast<std::int32_t>(words[0]);
    const std::int32_t capacity = static_cast<std::int32_t>(words[1]);
    if (count < capacity)
    {
        words[static_cast<std::size_t>(count) + 2U] = task_word(task);
        words[0] = static_cast<std::uint32_t>(count + 1);
        return task_list;
    }
    (void)Debug_Error("%s", qtask_list_full_message);
    for (;;)
    {
    }
}

// Native: QTaskList_Remove; retail VA 0x004C17C0
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
// Tools/evidence: Binary Ninja MCP (Pseudo C + disassembly; selected decompiler), Radare2 CFG and angr CFGFast; static capture build/native-analysis/0x004C17C0/capture.json, per-tool records docs/fidelity/evidence/0x004C17C0-*.json
// Behavior/CFG: seven blocks; `esi=[esp+8]; eax=0; ecx=[esi]; test ecx,ecx; jle error`; scans `edx=esi+8` with `cmp [edx],edi; je hit; inc eax; add edx,4; cmp eax,ecx; jl scan`; hit `dec ecx; [esi]=ecx; ecx=[esi+ecx*4+8]; [esi+eax*4+8]=ecx; retn` returns the removed index; miss pushes "Trying to remove unrigistered QTask from QTaskList" (0x00547F08), calls Debug_Error, `jmp $`
// Status/limits: STRUCTURALLY_VERIFIED; CFG 1.000 (BN 7 blocks = r2 7 blocks) and angr exact entry; the search/swap order and the preserved retail message spelling are source-reviewed, pending behavioural and paired runtime evidence
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
std::int32_t NATIVE_DECL QTaskList_Remove(void* task_list, void* task)
{
    std::uint32_t* const words = list_words(task_list);
    std::int32_t index = 0;
    const std::int32_t count = static_cast<std::int32_t>(words[0]);
    if (count > 0)
    {
        const std::uint32_t wanted = task_word(task);
        do
        {
            if (words[static_cast<std::size_t>(index) + 2U] == wanted)
            {
                words[0] = static_cast<std::uint32_t>(count - 1);
                words[static_cast<std::size_t>(index) + 2U] =
                    words[static_cast<std::size_t>(count) + 1U];
                return index;
            }
            ++index;
        } while (index < count);
    }
    (void)Debug_Error("%s", qtask_list_remove_message);
    for (;;)
    {
    }
}

} // extern "C"
