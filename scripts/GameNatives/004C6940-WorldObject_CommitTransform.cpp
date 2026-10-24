// Retail runtime observation: WorldObject_CommitTransform - 0x004C6940 - 102687 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Entry observed; generic model and orientation reads are invalid for some object types. Placement equality remains unverified.
// IGI native provenance: WorldObject_CommitTransform - 0x004C6940 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C6940-WorldObject_CommitTransform.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C6940-WorldObject_CommitTransform.hpp"
int WorldObject_CommitTransform(void* context, void* arg2, double* arg3, float arg4)

{
    int* item_2c = *(arg2 + 0x50);
    int result = Sys_ResolveSymbol_11(arg3, arg2 + 0x20);

    if (!result)
    {
    label_4c697e:
        int item_1c_1 = arg4;
        int temp_2 = *(context + 0xde0);
        int entity_2 = *arg3 + temp_2;
        int item_28_1 = temp_2;
        int temp_3 = *(context + 0xde4);
        int data_4 = *(context + 0xde8);
        int entity_4 = arg3[1] + temp_3;
        void* entity_6 = arg3[2] + data_4;
        int data_6 = *(arg2 + 0x20) + item_28_1;
        int entity_8 = *(arg2 + 0x28) + temp_3;
        item_28_1 = *(arg2 + 0x30);
        *(arg2 + 0x20) = *arg3;
        *(arg2 + 0x24) = *(arg3 + 4);
        *(arg2 + 0x28) = arg3[1];
        *(arg2 + 0x2c) = *(arg3 + 0xc);
        *(arg2 + 0x30) = arg3[2];
        *(arg2 + 0x34) = *(arg3 + 0x14);
        *(arg2 + 0x44) = arg4;
        int temp_13 = *item_2c;
        int result_1 = DynObj_LinkDynamicCube_7(data_6, entity_8, item_28_1 + data_4, entity_2, entity_4, entity_6);
        result = DynObj_LinkDynamicCube_7(entity_2 - item_1c_1, entity_4 - item_1c_1, entity_6 - item_1c_1, entity_2 + item_1c_1,
            entity_4 + item_1c_1, entity_6 + item_1c_1);

        if (result > result_1)
        {
            result_1 = result;
        label_4c6aac:
            DynObj_InsertHashItem_8(item_2c, arg2);

            if (temp_13 >= result_1)
            {
                int* temp_23 = item_2c;
                int entity_14 = 0x8000;
                void* i = temp_23[0xb];

                while (i)
                {
                    result_1 = *(i + 0x5c);
                    i = *(i + 0x58);
                    entity_14 |= result_1;
                }

                temp_23[0xe] = entity_14;
            }
            else
                DynObj_FindSymbol_8(context, result_1, temp_13, &item_2c);

            return DynObj_LookupHashItem_8(context, item_2c, arg2, result, entity_2, entity_4, entity_6);
        }

        if (temp_13 != result_1 || result_1 != result)
            goto label_4c6aac;
    }
    else
    {
        long double fpu_val_7_1 = arg4;
        long double f0_1 = *(arg2 + 0x44);
        fpu_val_7_1 - f0_1;
        result = (fpu_val_7_1 < f0_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f0_1) ? 1 : 0) << 0xa
            | (fpu_val_7_1 == f0_1 ? 1 : 0) << 0xe;

        if (!(*(&result + 1) & 0x40))
            goto label_4c697e;
    }

    return result;
}
