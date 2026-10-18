// IGI native provenance: SoundSys_PlaySoundSample_2 - 0x004E43D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E43D0-SoundSys_PlaySoundSample_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_PlaySoundSample_2.hpp"
struct status_msg* SoundSys_PlaySoundSample_2(float context, void* arg2, int* arg3, double* arg4, float* arg5, float* arg6, char arg7, char arg8, int arg9, int arg10, char* arg11)

{
    int kind;
    kind = g_a5e3f4;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    DynObj_LookupHashItem_6(arg2, result, DynObj_Missing(arg11));
    result->head[0x1c] = 0x3f800000;
    result->head[0x20] = 0x3f800000;
    result->head[0x24] = 0x3f800000;
    result->head[0x1d] = 0;
    result->head[0x1e] = 0;
    result->head[0x1f] = 0;
    result->head[0x21] = 0;
    result->head[0x22] = 0;
    result->head[0x23] = 0;
    result->head[0x25] = 0;
    void* __offset(status_msg, 0x5a4) data_1 = &result->head[0x169];
    int i_1 = 2;
    int i;

    do
    {
        *(ADJ(data_1) + 0x5a0) = 0;
        *(ADJ(data_1) + 0x5a4) = 0;
        data_1 += 8;
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    result->sprite[0x3c] = arg2;
    result->sprite[0x3d] = *(&arg2 + 1);
    result->sprite[0x3e] = *(&arg2 + 2);
    result->sprite[0x3f] = *(&arg2 + 3);
    *(result + 0x59c) = arg9;
    *(result + 0x5b0) = 0;

    if (arg7)
        *(result + 0x59c) = 1;

    *(result + 0x5b5) = 0;
    *(result + 0x5b4) = 0;
    double item_18 = (*arg3 + *arg4) * 0.5;
    double item_10 = (*(arg3 + 8) + arg4[1]) * 0.5;
    double item_8 = (*(arg3 + 0x10) + arg4[2]) * 0.5;
    DynObj_LinkDynamicCube_8(arg2, result, &item_18, Sys_DispatchOpcode_11(arg3, arg4), 1);

    if (!*arg3 && arg3[1] == 0xc113f2de)
    {
        *arg3 = 0;
        arg3[1] = 0xc113f2de;
    }

    int item_30_2 = arg10;
    SoundSys_Update3DPosition_2(result, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg11);
    return result;
}
