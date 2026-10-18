// IGI native provenance: SoundSys_SetMasterVolume_12 - 0x004E8220 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E8220-SoundSys_SetMasterVolume_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_SetMasterVolume_12.hpp"
int SoundSys_SetMasterVolume_12(int context, int arg2, char arg3)

{
    *(context + arg2 + 0xb1) = arg3;
    return arg3;
}
