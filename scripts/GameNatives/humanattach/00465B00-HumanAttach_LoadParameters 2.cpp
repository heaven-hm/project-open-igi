// IGI native provenance: HumanAttach_LoadParameters - 0x00465B00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00465B00-HumanAttach_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAttach_LoadParameters.h"
int HumanAttach_LoadParameters(void* context, int arg2)

{
    int item_c = 0;
    int item_8 = 0;
    int item_4 = 0;
    struct status_msg* data_2 =
        Vehicle_InitPlane_10(Script_FormatString(), context + 0x20, context + 0x70, context + 0x128, &item_c);
    unsigned int data_4 = Level_LoadLevel_3();
    int temp_1 = 0;
    temp_1 = data_2->head[7];
    int result = *(((data_4 * 0x180 + temp_1) << 2) + &g_a96ae0);

    if (result)
        result = result(data_2, arg2);

    *(context + 0x138) = 1;
    return result;
}
