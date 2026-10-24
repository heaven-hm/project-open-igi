// IGI native provenance: FX_SmokeEmit - 0x00480FE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00480FE0-FX_SmokeEmit.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_SmokeEmit.h"
struct status_msg* FX_SmokeEmit(int context, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6, int arg7, unsigned int arg8, unsigned int arg9, int* arg10, int arg11, int arg12, int* arg13, int arg14, int arg15, unsigned int arg16)

{
    float item_c = Sys_AllocateMemory_14(Level_SetMission_5()) - 0.5f;
    float item_8 = Sys_AllocateMemory_14(Level_SetMission_5()) - 0.5f;
    float item_4 = Sys_AllocateMemory_14(Level_SetMission_5()) - 0.300000012f;
    return FX_DebrisSpawn(context, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12,
        arg13, arg14, arg15, &item_c, 0x3fc90fdb, 0x43880000, 0x44aa0000, arg16);
}
