// IGI native provenance: ComputerButtonItem_Process_2 - 0x0046B660 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046B660-ComputerButtonItem_Process_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "ComputerButtonItem_Process_2.hpp"
struct status_msg* ComputerButtonItem_Process_2(float context, int arg2, int arg3, int arg4, int arg5, int arg6, char arg7, float arg8, float arg9, char arg10, void* arg11, char* arg12, unsigned int arg13, unsigned int arg14, unsigned int arg15, int arg16)

{
    int kind;
    kind = g_5be2b4;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    Physics_SolveConstraints_4(result, arg2, arg3, arg4, arg5, arg6, arg7, arg16);
    result->head[0x20] = arg10;
    result->head[0x22] = arg13;
    result->head[0x23] = arg14;
    result->head[0x24] = arg15;
    arg7 = Sys_CloseFileStream_17(arg13);
    arg10 = arg7;
    arg7 = Sys_ExecuteBytecode_17(arg13);
    char* dest_1 = arg12;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_1;
        dest_1 = &dest_1[1];
        i -= 1;

        if (!flag_0_1)
            break;
    }

    context = arg7;
    int entity_4 = ~i;
    long double fpu_val_7_6;

    if (entity_4 == 1)
    {
        fpu_val_7_6 = 0.0f;
        result->head[0x21] = 0;
        arg7 = 0;
    }
    else
    {
        int item_10_1 = entity_4 - 1;
        int item_14_1 = entity_4 - 1;
        int** data_7 = Sys_OpenFileStream_18(arg11, arg10 + arg8, arg9, arg12);
        result->head[0x21] = data_7;
        data_7[9] |= 0x40000;
        *(result->head[0x21] + 0xc) = 0x44029eb8;
        arg7 = Sys_CheckHeap_21(result->head[0x21]);
        arg7 = arg7;
        fpu_val_7_6 = Sys_AllocateMemory_22(result->head[0x21]);
    }

    result->head[0x1e] = arg8;
    result->head[0x17] = arg8;
    result->head[0x19] = arg7 + arg10;
    long double fpu_val_6_2 = context;
    fpu_val_6_2 - fpu_val_7_6;
    result->head[0x1f] = arg9;
    result->head[0x1d] = result;
    result->head[0x18] = arg9;

    if (!(*(&((fpu_val_6_2 < fpu_val_7_6 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_2, fpu_val_7_6) ? 1 : 0) << 0xa
            | (fpu_val_6_2 == fpu_val_7_6 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41))
        fpu_val_7_6 = context;

    result->head[0x1a] = fpu_val_7_6;
    return result;
}
