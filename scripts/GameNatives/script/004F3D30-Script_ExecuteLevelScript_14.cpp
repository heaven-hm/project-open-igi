// IGI native provenance: Script_ExecuteLevelScript_14 - 0x004F3D30 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004F3D30-Script_ExecuteLevelScript_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F3D30-Script_ExecuteLevelScript_14.hpp"
int Script_ExecuteLevelScript_14(void* context, int* arg2)

{
    int* item_28 = *(context + 0x2014c);
    void* data_1 = Script_ExecuteLevelScript_4(item_28);
    int result = *arg2;
    int item_14;
    memcpy(&item_14, arg2, 0x14);
    item_14 = result + 0xe8;
    memcpy(result, context + 0x6c, 0xe8);

    if (data_1)
    {
        unsigned int data_3 = Script_FormatString_15();
        int data_4 = 0;
        data_4 = *(data_1 + 0x1c);
        result = *(((data_3 * 0x180 + data_4) << 2) + &g_a96ae0);

        if (result)
        {
            item_28 = &item_14;
            return result(data_1, item_28);
        }
    }

    return result;
}
