// IGI native provenance: Render_SetupLighting_2 - 0x004D0DA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D0DA0-Render_SetupLighting_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_SetupLighting_2.h"
int Render_SetupLighting_2(void* context, int arg2)

{
    return *(*(*(context + 0xc) + 0x14) + arg2 * 0x14);
}
