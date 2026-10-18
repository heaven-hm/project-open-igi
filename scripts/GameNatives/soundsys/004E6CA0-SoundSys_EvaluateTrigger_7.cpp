// IGI native provenance: SoundSys_EvaluateTrigger_7 - 0x004E6CA0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004E6CA0-SoundSys_EvaluateTrigger_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_EvaluateTrigger_7.hpp"
int SoundSys_EvaluateTrigger_7(void* context, int arg2)

{
    int temp_val = 0;
    arg2 = arg2;
    temp_val = *(context + 0x1c);
    return (*(((*g_548818 * 0x180 + temp_val) << 2) + &g_a96ae0))(context, &arg2);
}
