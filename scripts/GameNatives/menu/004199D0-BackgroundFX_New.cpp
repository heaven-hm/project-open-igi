// IGI native provenance: BackgroundFX_New - 0x004199D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004199D0-BackgroundFX_New.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "BackgroundFX_New.hpp"
struct status_msg* BackgroundFX_New(void* context)

{
    int kind;
    kind = g_539af0;
    return QTask_HandleInput(context, kind, 0);
}
