// IGI native provenance: SoundSys_LoadSoundDef_4 - 0x004E6290 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E6290-SoundSys_LoadSoundDef_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_LoadSoundDef_4.h"
int* SoundSys_LoadSoundDef_4(int* context, int* arg2)

{
    int temp_val = *arg2;
    *context = 0;
    context[1] = temp_val;
    int** temp_1 = *arg2;

    if (temp_1)
        *temp_1 = context;

    *arg2 = context;
    return context;
}
