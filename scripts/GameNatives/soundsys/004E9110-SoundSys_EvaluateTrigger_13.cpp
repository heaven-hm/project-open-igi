// IGI native provenance: SoundSys_EvaluateTrigger_13 - 0x004E9110 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E9110-SoundSys_EvaluateTrigger_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_EvaluateTrigger_13.hpp"
int SoundSys_EvaluateTrigger_13(void* context, int arg2, int arg3, int arg4, int arg5)

{
    int data_1;
    int temp_val;
    temp_val = HIGHD(0xff - arg5);
    data_1 = LOWD(0xff - arg5);
    int dest_1 = (data_1 - temp_val) >> 1;
    int result;

    if (!dest_1)
        result = arg2;

    if (dest_1 || result || arg3 || arg4)
    {
        int entity_1 = Display_ClearSurfaces_13(context + 0x30, 1);

        if (dest_1)
        {
            int data_4 = dest_1 & 0xff;
            int entity_2 = *(context + 0x54);
            int item_30_1 = entity_2;
            int item_34_1 = entity_2;
            int item_38_1 = entity_2;
            int entity_3 = *(context + 0x48);
            int item_3c_1 = entity_3;
            entity_1 = Sys_ReadFileBlock_14(entity_3, *(context + 0x4c), *(context + 0x50), entity_2, data_4, data_4, data_4,
                0x10, 0);
        }

        result = arg2;

        if (result || arg3 || arg4)
        {
            int item_24_2 = entity_1;
            int item_28_2 = entity_1;
            int item_2c_2 = entity_1;
            int item_30_3 = entity_1;
            int item_34_3 = entity_1;
            int entity_4 = *(context + 0x4c);
            int item_38_3 = entity_4;
            int item_3c_3 = entity_4;
            result = Sys_ReadFileBlock_14(*(context + 0x48), entity_4, *(context + 0x50), *(context + 0x54),
                result & 0xff, arg3 & 0xff, arg4 & 0xff, 8, 0);
        }
    }

    return result;
}
