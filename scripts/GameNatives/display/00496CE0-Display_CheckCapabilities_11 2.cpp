// IGI native provenance: Display_CheckCapabilities_11 - 0x00496CE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00496CE0-Display_CheckCapabilities_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CheckCapabilities_11.h"
int Display_CheckCapabilities_11(char* context, void* arg2, int arg3)

{
    int dest_val = 0;
    short context_val = *(*(arg2 + 0xc) + 0x18);
    int entity_val = context_val;
    int data_3;
    int temp_val;
    temp_val = HIGHD(entity_val * arg3);
    data_3 = LOWD(entity_val * arg3);
    int result = (data_3 - temp_val) >> 1;

    if ((*(arg2 + 0x24) * entity_val) & 1)
        result += 1;
    else if (arg3 & 1 && context_val & 1)
    {
        dest_val = 1;
        result += 1;
    }

    if (result <= 0)
        return result;

    Resource_Unpack(*(arg2 + 0x18), 0x5ca674, result);
    Sys_ResolveSymbol_4(*(arg2 + 0x18));

    if (dest_val)
    {
        Sys_ExecuteBytecode_4(*(arg2 + 0x18), 0xffffffff, 1);
        Sys_ResolveSymbol_4(*(arg2 + 0x18));
    }

    return Display_SwapBuffers_10(0x5ca674, context, arg3, arg2 + 0x40);
}
