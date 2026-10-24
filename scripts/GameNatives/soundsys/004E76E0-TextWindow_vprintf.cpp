// IGI native provenance: SoundSys_Update3DPosition_10 - 0x004E76E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E76E0-SoundSys_Update3DPosition_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_Update3DPosition_10.hpp"
int SoundSys_Update3DPosition_10(int* context, char* arg2, int arg3)

{
    String_VFormat(0xa5ea78, arg2, arg3);
    return SoundSys_SetMasterVolume_10(context, 0xa5ea78);
}
