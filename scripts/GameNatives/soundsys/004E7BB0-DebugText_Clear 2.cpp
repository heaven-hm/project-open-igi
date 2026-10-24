// IGI native provenance: DebugText_Clear - 0x004E7BB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E7BB0-DebugText_Clear.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DebugText_Clear.h"
int DebugText_Clear()

{
    char result = g_a5ea75;

    if (result)
        result = SoundSys_EvaluateTrigger_9(g_a5ebd0);

    return result;
}
