// IGI native provenance: Script_GraphnodeIdAlreadyUs - 0x004F9510 - CFG score 85.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9510-Script_GraphnodeIdAlreadyUs.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F9510-Script_GraphnodeIdAlreadyUs.hpp"
int* Script_GraphnodeIdAlreadyUs(void* context, int arg2, int arg3, int arg4, float arg5, int arg6, int arg7)

{
    if (arg2 >= 0x3e8)
        return 0;

    if (arg2 >= *(context + 0x58))
        Script_ParseEnumReal32_23(context, arg2);

    if (!Script_ExecuteLevelScript_23(context, arg2))
    {
        int data_2 = Script_UndefinedGraphnodeda(context);
        int entity_val = *(context + 0x6c);
        void* context_1 = data_2 * arg2;
        long double fpu7 = arg5;
        *(context_1 + entity_val + 0x28) = fpu7 + fpu7;
        int dest_val = arg7;
        int* result = context_1 + entity_val;
        result[9] = arg5;
        result[8] = arg4;
        int i = 0xffffffff;
        *result = arg2;
        result[0xb] = 0xffffffff;

        while (i)
        {
            bool flag_0_1 = 0 != *dest_val;
            dest_val += 1;
            i -= 1;

            if (!flag_0_1)
                break;
        }

        int entity_2 = ~i;
        int source_2;
        int dest_3;
        dest_3 = memcpy(&result[0xd], dest_val - entity_2, entity_2 & 0xfffffffc);
        memcpy(dest_3, source_2, entity_2 & 3);
        memcpy(&result[2], arg3, 0x18);
        result[0x4d] = context;
        result[0xc] = Sys_CheckHeap_22(&result[0xd]);
        result[0x4e] = arg6;
        int data_6 = 0;
        memset(&result[0x4f], 0, 0x100);
        data_6 = g_54e36f;
        int data_7 = 0;
        data_7 = *(context + 0x1c);
        int data_8 = *(((data_6 * 0x180 + data_7) << 2) + &g_a96ae0);

        if (data_8)
            data_8(context, result);

        *(context + 0x5c) += 1;
        return result;
    }

    Error_Show("GRAPHNODE ID ALREADY USED");

    while (true)
        /* nop */
}
