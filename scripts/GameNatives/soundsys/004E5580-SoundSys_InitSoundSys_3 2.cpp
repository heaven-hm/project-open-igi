// IGI native provenance: SoundSys_InitSoundSys_3 - 0x004E5580 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E5580-SoundSys_InitSoundSys_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_InitSoundSys_3.h"
int SoundSys_InitSoundSys_3(void* context, int arg2, int arg3)

{
    int context_val = 0;
    context_val = *((((arg3 << (*(context + 4) & 0x1f)) + (arg2 & 0xfffffff8)) >> 3) + context + 0xc);
    return context_val >> (arg2 & 7) & 1;
}
