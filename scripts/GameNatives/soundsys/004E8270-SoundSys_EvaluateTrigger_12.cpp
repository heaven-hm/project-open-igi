// IGI native provenance: SoundSys_EvaluateTrigger_12 - 0x004E8270 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E8270-SoundSys_EvaluateTrigger_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_EvaluateTrigger_12.hpp"
int SoundSys_EvaluateTrigger_12(void* context, int arg2)

{
    *(context + 0xbc) = arg2;
    return arg2;
}
