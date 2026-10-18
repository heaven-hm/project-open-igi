// IGI native provenance: SoundDefTriggerOnce_Action - 0x004E9E20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E9E20-SoundDefTriggerOnce_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundDefTriggerOnce_Action.h"
float SoundDefTriggerOnce_Action(int context, int* arg2) {
    float result = Display_OpenDevice_12(*(context + 0x130), *(context + 0x134));
    result = SoundSys_SetFrequency_4() * result;

    if (result > *arg2)
        *arg2 = result;

    return result;
}
