// IGI native provenance: SoundSys_AttachLipsync_11 - 0x004E8110 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E8110-SoundSys_AttachLipsync_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_AttachLipsync_11.h"
struct status_msg* SoundSys_AttachLipsync_11(void* context, unsigned int arg2, unsigned int arg3, unsigned int arg4, unsigned int arg5, unsigned int arg6, unsigned int arg7, unsigned int arg8 , unsigned int arg9, unsigned int arg10, unsigned int arg11)

{
    struct status_msg* result = QTask_HandleInput(context, SoundSys_EvaluateTrigger_11(), 0);
    result->sprite[0xc] = 1;
    result->sprite[0xd] = 0;
    result->sprite[0xe] = 0;
    result->sprite[0xf] = 0;
    result->sprite[8] = 1;
    result->sprite[9] = 0;
    result->sprite[0xa] = 0;
    result->sprite[0xb] = 0;
    result->text = 0x1010101;
    result->sprite[0] = 1;
    result->sprite[1] = 1;
    result->sprite[2] = 1;
    result->sprite[3] = 1;
    result->sprite[4] = 1;
    Terrain_BuildMipMaps_7(&result->head[0x2b]);
    g_a5ec60;
    g_a5ec60 += 1;
    result->head[9] = arg3;
    result->head[8] = arg2;
    result->head[0x28] = arg8;
    result->head[0xa] = arg4;
    result->head[0xb] = arg5;
    result->head[0xc] = arg11;
    result->head[0x29] = arg9;
    result->head[0x2a] = arg10;
    result->head[0x13] = arg6;
    result->head[0x14] = arg7;
    Display_SyncVSync_13(&result->head[0x15], 0x100);
    int* data_7 = Sys_FreeMemory_19(0x1388);
    result->sprite[0x10] = data_7;
    result->sprite[0x11] = *(&data_7 + 1);
    result->sprite[0x12] = *(&data_7 + 2);
    result->sprite[0x13] = *(&data_7 + 3);
    return result;
}
