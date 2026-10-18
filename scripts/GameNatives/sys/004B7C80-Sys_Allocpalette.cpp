// IGI native provenance: Sys_Allocpalette - 0x004B7C80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7C80-Sys_Allocpalette.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7C80-Sys_Allocpalette.hpp"
int Sys_Allocpalette()

{
    g_a84a48 = Sys_RendermodeFailedToCr_2("AllocPalette");
    g_a84a40 = Sys_RendermodeFailedToCr_2("DeAllocPalette");
    int result = Sys_RendermodeFailedToCr_2("DownloadPalette");
    g_a84a44 = result;
    return result;
}
