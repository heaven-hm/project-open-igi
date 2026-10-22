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
// Tools/evidence: Binary Ninja MCP and Radare2 VA 0x004037C0..0x0040385E; scripts/native_unicorn_runner.py differential; paired Frida capture retail/candidate call counts.
// Behavior/CFG: reads the action count at 0x00BC2380, takes the action slot from 0x00BC2358 at a 0x345-dword stride, computes slot=(count-1)*115+action, writes the three frame arguments (reader 0x004B8A20) to 0x00BC25BC at a three-dword stride masking the third to 30 bits, increments the action slot, then copies a 24-byte block {0,0,0,0,0x00567C74,uninitialised} to its destination and returns that destination.
// Status/limits: VERIFIED; the table arithmetic, mask and returned block are retail-equivalent. Deviation: the retail caller supplies the destination buffer, while this port fills a private static block and returns it, because its config decoder discards the result.
std::uint32_t* IGI_RETAIL_CDECL GOInRemap(void* qvm_context)
{
    const std::uint32_t count = InputRemap_ActionCount;
    std::uint32_t* const action_slot =
        InputRemap_ActionTable + count * igi::retail::config::action_table_stride;
    const std::uint32_t action = *action_slot;
    const std::uint32_t slot = (count - 1U) * 115U + action;
    std::uint32_t* const entry = InputRemap_Table + slot * igi::retail::config::remap_table_stride;

    // The retail reads the arguments through the frame reader at 0x004B8A20,
    // which returns the argument value of the frame at the given index.
    const auto* const frame = static_cast<const igi::retail::config::QvmFrameView*>(qvm_context);
    entry[0] = frame->argument_values[0];
    entry[1] = frame->argument_values[1];
    entry[2] = frame->argument_values[2] & 0x3FFFFFFFU;

    // Retail 0x00403824: increment the action slot it just read.
    *action_slot = action + 1U;

    // Retail 0x0040382E..0x00403858: a six-dword block of four zeros and the
    // 0x00567C74 descriptor, copied to the destination and returned. Retail
    // leaves dwords 1 and 5 uninitialised; the port writes zero into them.
    static std::uint32_t result_block[6] = {0U, 0U, 0U, 0U, 0x00567C74U, 0U};
    return result_block;
}

} // extern "C"

#undef IGI_RETAIL_CDECL
