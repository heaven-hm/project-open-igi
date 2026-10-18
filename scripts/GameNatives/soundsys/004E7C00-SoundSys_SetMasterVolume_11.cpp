// IGI native provenance: SoundSys_SetMasterVolume_11 - 0x004E7C00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E7C00-SoundSys_SetMasterVolume_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_SetMasterVolume_11.hpp"
int SoundSys_SetMasterVolume_11(char* context, int arg2)

{
    char result = g_a5ea75;

    if (!result)
        return result;

    return SoundSys_Update3DPosition_10(g_a5ebd0, context, arg2);
}
