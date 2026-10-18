// IGI native provenance: SoundSys_InitSoundSys_8 - 0x004E6D20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E6D20-SoundSys_InitSoundSys_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_InitSoundSys_8.hpp"
int* SoundSys_InitSoundSys_8(int context)

{
    return DynObj_QtasklistIsFull(g_a5e60c, context);
}
