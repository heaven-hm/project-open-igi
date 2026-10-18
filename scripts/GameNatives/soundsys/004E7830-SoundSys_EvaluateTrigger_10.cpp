// IGI native provenance: SoundSys_EvaluateTrigger_10 - 0x004E7830 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E7830-SoundSys_EvaluateTrigger_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_EvaluateTrigger_10.hpp"
int SoundSys_EvaluateTrigger_10(void* context, char arg2)

{
    int result = arg2;
    *(context + 4) = result;
    return result;
}
