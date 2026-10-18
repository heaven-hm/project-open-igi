// IGI native provenance: SoundSys_InitSoundSys_11 - 0x004E7B80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E7B80-SoundSys_InitSoundSys_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_InitSoundSys_11.h"
int SoundSys_InitSoundSys_11(int context, int arg2)

{
    char result = g_a5ea75;

    if (!result)
        return result;

    return SoundSys_LoadSoundDef_9(g_a5ebd0, context, arg2);
}
