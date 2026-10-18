// IGI native provenance: Render_SetupLighting_9 - 0x004D6110 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D6110-Render_SetupLighting_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_SetupLighting_9.h"
int Render_SetupLighting_9(float context, int arg2, int arg3, int arg4)

{
    return Bones_PlayAnimation(context, arg2, arg3, arg4, 0.0f);
}
