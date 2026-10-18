// IGI native provenance: DynObj_UpdateDynTree_8 - 0x004C6E70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C6E70-DynObj_UpdateDynTree_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_UpdateDynTree_8.h"
int DynObj_UpdateDynTree_8(void* context, void* arg2, double* arg3, double* arg4, double* arg5)

{
    int* item_4c = *(arg2 + 0x50);
    int result = Sys_ResolveSymbol_11(arg3, arg2 + 0x20);

    if (!result)
    {
        int temp_8 = *(arg2 + 0x20) + *(context + 0xde0);
        int temp_10 = *(arg2 + 0x28) + *(context + 0xde4);
        int temp_12 = *(arg2 + 0x30) + *(context + 0xde8);
        int context_4 = *arg3 + *(context + 0xde0);
        int context_6 = arg3[1] + *(context + 0xde4);
        void* context_8 = arg3[2] + *(context + 0xde8);
        int context_10 = *arg4 + *(context + 0xde0);
        int context_12 = arg4[1] + *(context + 0xde4);
        int context_14 = arg4[2] + *(context + 0xde8);
        int context_16 = *arg5 + *(context + 0xde0);
        int context_18 = arg5[1] + *(context + 0xde4);
        int context_20 = arg5[2] + *(context + 0xde8);
        *(arg2 + 0x20) = *arg3;
        *(arg2 + 0x24) = *(arg3 + 4);
        *(arg2 + 0x28) = arg3[1];
        *(arg2 + 0x2c) = *(arg3 + 0xc);
        *(arg2 + 0x30) = arg3[2];
        *(arg2 + 0x34) = *(arg3 + 0x14);
        long double fpu_val_7_14 = *arg5 - *arg4;
        long double fpu_val_6_2 = arg5[1] - arg4[1];
        long double fpu_val_5_2 = arg5[2] - arg4[2];
        int* data_4 = item_4c;
        *(arg2 + 0x44) =
            sqrtl(fpu_val_5_2 * fpu_val_5_2 + fpu_val_6_2 * fpu_val_6_2 + fpu_val_7_14 * fpu_val_7_14) * 0.5f;
        int entity_5 = *data_4;
        int result_3 = DynObj_LinkDynamicCube_7(temp_8, temp_10, temp_12, context_4, context_6, context_8);
        result = DynObj_LinkDynamicCube_7(context_10, context_12, context_14, context_16 - 1, context_18 - 1, context_20 - 1);
        int result_2 = result_3;

        if (result > result_2)
        {
            int result_1 = result;
            result_2 = result;
        label_4c70f1:
            DynObj_InsertHashItem_8(item_4c, arg2);

            if (entity_5 < result_2)
                DynObj_FindSymbol_8(context, result_2, entity_5, &item_4c);

            return DynObj_LookupHashItem_8(context, item_4c, arg2, result, context_4, context_6, context_8);
        }

        if (entity_5 != result_2 || result_2 != result)
            goto label_4c70f1;
    }

    return result;
}
