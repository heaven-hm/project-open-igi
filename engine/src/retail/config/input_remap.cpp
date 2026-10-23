#include "igi/retail/config/input_remap.hpp"

#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

extern "C" {

std::uint32_t InputRemap_ActionCount = 0U;
std::uint32_t InputRemap_ActionTable[igi::retail::config::action_table_size] = {};
std::uint32_t InputRemap_Table[igi::retail::config::remap_table_size] = {};

// Native: GOInRemap; retail VA 0x004037C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP, Radare2 and angr CFGFast on retail IGI.EXE; VA 0x004037C0..0x00403860.
// Behavior/CFG: one block. Reads the action count at 0x00BC2380 and indexes the action table at 0x00BC2358 with the 345-dword stride, so esi = &InputRemap_ActionTable[count]. Reads the action slot there and computes the remap slot as (count - 1) * 115 + action. Reads script arguments 0, 1 and 2 through the frame reader at 0x004B8A20 and stores them at 0x00BC25BC + slot * 3 dwords, masking argument 2 to the low 30 bits. Then increments the action slot (*esi += 1) and builds a six-dword block on the stack: dword 0 = 0, dword 2 = 0, dword 3 = 0, dword 4 = 0x00567C74; dwords 1 and 5 are never written. Copies the block to the destination pointer passed in arg1 with rep movsd x6 and returns that destination pointer in EAX.
// Status/limits: STRUCTURALLY_VERIFIED; the table addresses, strides, arithmetic, action-slot increment, block contents and returned destination pointer are checked against the retail disassembly. The script-argument read is modelled by the port's frame view instead of the 0x004B8A20 reader, and retail leaves block dwords 1 and 5 uninitialised (stack residue) while the port writes zero; both are recorded engine-boundary deviations. No paired runtime capture exists, so RUNTIME_TESTED/VERIFIED is not claimed.
std::uint32_t* IGI_RETAIL_CDECL GOInRemap(std::uint32_t* result, std::int32_t unused,
                                          void* qvm_context)
{
    (void)unused;
    // 0x004037C3..0x004037EA: count, action slot and its address (esi).
    const std::uint32_t count = InputRemap_ActionCount;
    std::uint32_t* const action_slot =
        InputRemap_ActionTable + static_cast<std::size_t>(count) * igi::retail::config::action_table_stride;
    const std::uint32_t action = *action_slot;

    // 0x004037F1..0x004037F8: (count - 1) * 115 + action, then * 3 dwords.
    const std::uint32_t slot = (count - 1U) * 115U + action;
    std::uint32_t* const entry =
        InputRemap_Table + static_cast<std::size_t>(slot) * igi::retail::config::remap_table_stride;

    // 0x004037FF..0x00403821: argument 0, argument 1, argument 2 masked.
    const auto* const frame = static_cast<const igi::retail::config::QvmFrameView*>(qvm_context);
    entry[0] = frame->argument_values[0];
    entry[1] = frame->argument_values[1];
    entry[2] = frame->argument_values[2] & 0x3FFFFFFFU;

    // 0x00403824..0x0040382C: increment the action slot the call just read.
    *action_slot = action + 1U;

    // 0x0040382E..0x00403858: build the six-dword result block and copy it to
    // the caller's destination. Retail leaves dwords 1 and 5 untouched; the
    // port zeroes them so the host block is deterministic.
    const std::uint32_t block[igi::retail::config::remap_result_dword_count] = {
        0U,
        0U,
        0U,
        0U,
        igi::retail::config::remap_result_type_descriptor,
        0U,
    };
    std::memcpy(result, block, sizeof(block));

    // rep movsd leaves arg1 in EAX; the routine returns the destination pointer.
    return result;
}

} // extern "C"

#undef IGI_RETAIL_CDECL
