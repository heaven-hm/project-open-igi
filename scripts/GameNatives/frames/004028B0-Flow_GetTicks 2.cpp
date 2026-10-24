// IGI native provenance: Flow_GetTicks - 0x004028B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004028B0-Flow_GetTicks.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Flow_GetTicks.h"
unsigned int Flow_GetTicks()

// Sig: int Flow_GetTicks(void)
// Note: Returns the update-loop counter at game-context field +0x34. Frames_Set
// resets it and the retail main loop increments it once per simulation/update
// pass; gameplay timeout code compares against this value.
{
    return g_frames_ctx->unk34;
}
