// IGI native provenance: Render_BindTexture_2 - 0x004D0DC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D0DC0-Render_BindTexture_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_BindTexture_2.h"
int Render_BindTexture_2(void* context, int arg2, int arg3)

{
    int result = *(*(context + 0xc) + 0x14);
    *(result + arg2 * 0x14 + 4) = arg3;
    return result;
}
