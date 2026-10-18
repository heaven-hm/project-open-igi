// IGI native provenance: Sys_PopStack_22 - 0x004B8060 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B8060-Sys_PopStack_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_PopStack_22.h"
int Sys_PopStack_22(int context, int arg2)

{
    int item_8 = arg2;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    memcpy(context, &item_18, 0x18);
    return context;
}
