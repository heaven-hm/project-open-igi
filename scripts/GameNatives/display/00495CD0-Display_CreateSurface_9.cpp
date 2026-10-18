// IGI native provenance: Display_CreateSurface_9 - 0x00495CD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00495CD0-Display_CreateSurface_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CreateSurface_9.hpp"
int Display_CreateSurface_9(int* context, int arg2, int arg3)

{
    int* flagP = context;
    int result = 0;
    int dest_val = arg3;

    if ((*(*flagP + 0x2c))(flagP, arg2, dest_val, &context, &arg3, 0, 0, 0))
        return 0;

    int data_2 = g_5ca1a0;
    int source_val = g_5ca19c;

    if (source_val >= data_2 + dest_val)
    {
        Resource_Unpack(g_5ca194, context, dest_val);
        g_5ca1a0 += dest_val;
    }
    else
    {
        void* source_1 = source_val - data_2;
        Resource_Unpack(g_5ca194, context, source_1);
        Sys_ExecuteBytecode_4(g_5ca194, 0x2c, 0);
        void* dest_1 = dest_val - source_1;
        Resource_Unpack(g_5ca194, source_1 + context, dest_1);
        g_5ca1a0 = dest_1;
        result = 1;
    }

    (*(*flagP + 0x4c))(flagP, context, arg3, 0, 0);
    return result;
}
