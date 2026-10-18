// IGI native provenance: SoundSys_PlaySoundSample_17 - 0x004ECEA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ECEA0-SoundSys_PlaySoundSample_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_PlaySoundSample_17.h"
void* SoundSys_PlaySoundSample_17(int* context, float arg2, float arg3, float arg4, float arg5, int arg6)

{
    SoundSys_Update3DPosition_17(context, arg2, arg3, arg4, arg5);
    return SoundSys_StopSoundSample_16(context, arg6);
}
