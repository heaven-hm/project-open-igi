// IGI native provenance: Sys_ReadFileBlock_22 - 0x004B7F00 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B7F00-Sys_ReadFileBlock_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ReadFileBlock_22.h"
int Sys_ReadFileBlock_22(int context, int arg2)

{
    int data_1 = *arg2;
    int item_18 = data_1;
    char* item_8 = &g_567c74;
    double item_10 = data_1;
    memcpy(context, &item_18, 0x18);
    return context;
}
