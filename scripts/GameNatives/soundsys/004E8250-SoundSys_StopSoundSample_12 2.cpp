// IGI native provenance: SoundSys_StopSoundSample_12 - 0x004E8250 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E8250-SoundSys_StopSoundSample_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_StopSoundSample_12.h"
int SoundSys_StopSoundSample_12(void* context, char arg2)

{
    int result = arg2;
    *(context + 0xc0) = result;
    return result;
}
