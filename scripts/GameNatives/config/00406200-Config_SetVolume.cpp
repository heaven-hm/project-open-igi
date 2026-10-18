// IGI native provenance: Config_SetVolume - 0x00406200 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00406200-Config_SetVolume.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00406200-Config_SetVolume.hpp"
int Config_SetVolume(int context)

{
    int result = g_bc2384 * 0x159;
    (&g_bc28bc)[result] = context;
    return result;
}
