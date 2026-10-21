#include "igi/retail/config/input_remap.hpp"

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
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004037C0..0x00403822.
// Behavior/CFG: reads the action count at 0x00BC2380, takes the action's slot
//           number from the action table at 0x00BC2358 with a 0x345 dword
//           stride, and computes the remap slot as (count - 1) * 115 + slot. It
//           reads the three script arguments with the frame reader at
//           0x004B8A20, stores them at 0x00BC25BC with a three dword stride,
//           and returns the third argument masked to the low 30 bits.
// Status/limits: STRUCTURALLY_VERIFIED; the table addresses, the strides, the
//           slot arithmetic and the mask are the retail's.
std::uint32_t IGI_RETAIL_CDECL GOInRemap(void* qvm_context)
{
    const std::uint32_t count = InputRemap_ActionCount;
    const std::uint32_t action = InputRemap_ActionTable[count * igi::retail::config::action_table_stride];
    const std::uint32_t slot = (count - 1U) * 115U + action;
    std::uint32_t* const entry = InputRemap_Table + slot * igi::retail::config::remap_table_stride;

    // The retail reads the arguments through the frame reader at 0x004B8A20,
    // which returns the argument value of the frame at the given index.
    const auto* const frame = static_cast<const igi::retail::config::QvmFrameView*>(qvm_context);
    entry[0] = frame->argument_values[0];
    entry[1] = frame->argument_values[1];
    entry[2] = frame->argument_values[2];
    return entry[2] & 0x3FFFFFFFU;
}

} // extern "C"

#undef IGI_RETAIL_CDECL
