// IGI native provenance: SoundSys_PlaySoundSample_16 - 0x004EC190 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004EC190-SoundSys_PlaySoundSample_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_PlaySoundSample_16.h"
int SoundSys_PlaySoundSample_16(int* context, int arg2, int arg3, int arg4, int arg5, int arg6)

{
    *context = arg2;
    context[1] = arg3;
    context[2] = 0;
    context[3] = 0xffffffff;
    context[4] = arg4;
    context[5] = *(arg3 + (arg4 << 2));
    context[6] = arg5;
    context[8] = arg6;
    context[7] = 0xffffffff;
    memset(&context[9], 0, 0x28);
    return 0;
}
