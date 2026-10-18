// IGI native provenance: Sys_DispatchOpcode_22 - 0x004B7FE0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B7FE0-Sys_DispatchOpcode_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_DispatchOpcode_22.h"
int Sys_DispatchOpcode_22(int context, double* arg2)

{
    int temp_val = *arg2;
    int item_c = *(arg2 + 4);
    int item_18 = *arg2;
    int item_10 = temp_val;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
