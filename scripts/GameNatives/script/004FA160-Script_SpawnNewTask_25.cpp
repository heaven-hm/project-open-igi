// IGI native provenance: Script_SpawnNewTask_25 - 0x004FA160 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA160-Script_SpawnNewTask_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FA160-Script_SpawnNewTask_25.hpp"
int Script_SpawnNewTask_25(void* context, void* arg2)

{
    int item_258;
    Script_FormatString_26(arg2, &item_258);
    Script_FormatString_26(arg2, context + 0x58);
    int item_254;
    Script_FormatString_26(arg2, &item_254);
    void* data_val = *(context + 0x6c);

    if (data_val)
    {
        Sys_ExecuteBytecode_2(data_val);
        *(context + 0x6c) = 0;
    }

    void* data_3 = Mem_Alloc(Script_UndefinedGraphnodeda(context) * *(context + 0x58), 4);
    int temp_val = *(context + 0x58);
    *(context + 0x6c) = data_3;
    Script_ParseEnumString_23(context, temp_val);
    unsigned int data_5 = Script_EvaluateCondition_22();
    int entity_val = 0;
    entity_val = *(context + 0x1c);
    int data_9 = *(((data_5 * 0x180 + entity_val) << 2) + &g_a96ae0);

    if (data_9)
        data_9(context, 0);

    int context_1 = 0;
    int item_25c;
    int item_250;
    int* item_24c;

    if (item_258 > 0)
    {
        do
        {
            item_25c = 0xffffffff;
            int item_240;
            Script_FormatString_26(arg2, &item_240);
            void item_238;
            Script_ParseEnumReal32_27(arg2, &item_238);
            float item_220;
            Script_SpawnNewTask_27(arg2, &item_220);
            float item_21c[0x4];
            Script_SpawnNewTask_27(arg2, &item_21c);

            if (Script_CallNativeHandler_27(arg2) == 6)
                Script_SpawnNewTask_27(arg2, &item_250);

            if (Script_CallNativeHandler_27(arg2) == 5)
                Script_FormatString_26(arg2, &item_25c);

            int data_13 = item_25c;
            int item_108;

            if (data_13 != 0xffffffff)
                item_108 = DynObj_RemoveSymbol_11(g_a44344, data_13);

            int item_26c_7 = 0x100;
            void item_20c;
            Script_ParseEnumString_27(arg2, &item_20c);
            int item_278_3 = 0;
            int* data_17 =
                Script_GraphnodeIdAlreadyUs(context, item_240, &item_238, item_220, item_21c[0], item_108, &item_20c);

            if (data_17)
            {
                void* item_248_1 = arg2;
                item_24c = data_17;
                unsigned int data_19 = Script_ParseEnumReal32_22();
                int temp_3 = 0;
                temp_3 = *(context + 0x1c);
                int data_20 = *(((data_19 * 0x180 + temp_3) << 2) + &g_a96ae0);

                if (data_20)
                    data_20(context, &item_24c);
            }

            context_1 += 1;
        } while (context_1 < item_258);
    }

    int data_23 = item_254;
    int context_2 = 0;

    if (data_23 > 0)
    {
        do
        {
            Script_FormatString_26(arg2, &item_250);
            Script_FormatString_26(arg2, &item_24c);
            Script_FormatString_26(arg2, &item_25c);
            Script_SpawnNewTask_24(context, item_250, item_24c, item_25c);
            data_23 = item_254;
            context_2 += 1;
        } while (context_2 < data_23);
    }

    int temp_5 = item_258;
    *(context + 0x60) = data_23;
    *(context + 0x5c) = temp_5;
    void* item_244 = arg2;
    unsigned int data_25 = Script_SpawnNewTask_22();
    int entity_6 = 0;
    entity_6 = *(context + 0x1c);
    int result = *(((data_25 * 0x180 + entity_6) << 2) + &g_a96ae0);

    if (!result)
        return result;

    return result(context, &item_244);
}
