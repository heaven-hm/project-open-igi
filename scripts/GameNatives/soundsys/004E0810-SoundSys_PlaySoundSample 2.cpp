// IGI native provenance: SoundSys_PlaySoundSample - 0x004E0810 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004E0810-SoundSys_PlaySoundSample.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_PlaySoundSample.h"
int SoundSys_PlaySoundSample(int context, int arg2)

{
    memcpy(context + 0x70, arg2, 0x28);
    return context;
}
