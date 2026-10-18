// IGI native provenance: Env_TriggerAlarm_8 - 0x00446450 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00446450-Env_TriggerAlarm_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00446450-Env_TriggerAlarm_8.hpp"
int Env_TriggerAlarm_8(int* context, int arg2)

{
    *context = arg2 + 0x20;
    context[1] = arg2 + 0x70;
    return arg2 + 0x70;
}
