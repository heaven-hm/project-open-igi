// IGI native provenance: SoundDefSound_OnEvent - 0x004E8BD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E8BD0-SoundDefSound_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundDefSound_OnEvent.h"
float SoundDefSound_OnEvent(int context, int* arg2) {
    float result = Display_OpenDevice_12(*(context + 0x128), *(context + 0x12c));
    result = SoundSys_SetFrequency_4() * result;

    if (result > *arg2)
        *arg2 = result;

    return result;
}
