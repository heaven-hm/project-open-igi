// IGI native provenance: SoundSys_SetMasterVolume_19 - 0x004EE8E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EE8E0-SoundSys_SetMasterVolume_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_SetMasterVolume_19.hpp"
char* SoundSys_SetMasterVolume_19(int* context, char* arg2, short arg3, short arg4)

{
    *(arg2 + 0xc) = arg3;
    *(arg2 + 0xe) = arg4;
    *arg2 = 0;
    arg2[1] = 0;
    arg2[2] = 2;
    arg2[3] = 0;
    arg2[4] = 0;
    arg2[5] = 0;
    arg2[6] = 0;
    arg2[7] = 0;
    *(arg2 + 8) = 0;
    *(arg2 + 0xa) = 0;
    arg2[0x10] = 0x18;
    arg2[0x11] = 0x20;

    if (!context)
        return arg2;

    return Sys_CloseFileStream_4(context, arg2, 0x12);
}
