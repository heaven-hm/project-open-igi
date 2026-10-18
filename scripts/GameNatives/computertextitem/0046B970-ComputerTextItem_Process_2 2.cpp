// IGI native provenance: ComputerTextItem_Process_2 - 0x0046B970 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046B970-ComputerTextItem_Process_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "ComputerTextItem_Process_2.h"
struct status_msg* ComputerTextItem_Process_2(void* context, int arg2, int arg3, int arg4, int arg5, int arg6, char arg7, unsigned int arg8, unsigned int arg9, void* arg10, char* arg11, unsigned int arg12, int arg13)

{
    int kind;
    kind = g_5be2bc;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    Physics_SolveConstraints_4(result, arg2, arg3, arg4, arg5, arg6, arg7, arg13);
    int** data_4 = Sys_OpenFileStream_18(arg10, arg8, arg9, arg11);
    result->head[0x1e] = data_4;
    data_4[9] |= 0x40000;
    *(result->head[0x1e] + 0xc) = 0x44029eb8;
    result->head[0x1f] = arg12;
    unsigned int temp_5 = result->head[0x1e];
    result->head[0x20] = arg8;
    result->head[0x21] = arg9;
    arg7 = Sys_CheckHeap_21(temp_5);
    unsigned int data_7 = result->head[0x1e];
    result->head[0x22] = arg7;
    int data_8 = Sys_AllocateMemory_22(data_7);
    unsigned int entity_8 = result->head[0x20];
    unsigned int temp_6 = result->head[0x21];
    arg7 = data_8;
    long double fpu_val_7_1 = arg7;
    result->head[0x19] = result->head[0x22];
    result->head[0x1d] = result;
    result->head[0x17] = entity_8;
    result->head[0x23] = fpu_val_7_1;
    result->head[0x1a] = fpu_val_7_1;
    result->head[0x18] = temp_6;
    return result;
}
