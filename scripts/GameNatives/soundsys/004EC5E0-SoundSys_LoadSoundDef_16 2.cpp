// IGI native provenance: SoundSys_LoadSoundDef_16 - 0x004EC5E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EC5E0-SoundSys_LoadSoundDef_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_LoadSoundDef_16.h"
void* SoundSys_LoadSoundDef_16(int* context, float arg2, float arg3, float arg4, float arg5)

{
    *context = arg2;
    context[3] = 1.0f / arg3;
    context[4] = 1.0f / arg4;
    context[5] = 1.0f / arg5;
    context[1] = 1.0f / arg2;
    return SoundSys_AttachLipsync_16(context);
}
