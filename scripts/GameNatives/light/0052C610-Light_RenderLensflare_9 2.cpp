// IGI native provenance: Light_RenderLensflare_9 - 0x0052C610 - CFG score 95.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052C610-Light_RenderLensflare_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_RenderLensflare_9.h"
struct status_msg* Light_RenderLensflare_9(void* context, int arg2, int arg3)

{
    int kind = QHash_ValueGet();
    struct status_msg* result;

    if (kind)
    {
        int kind_1;
        kind_1 = g_a8454c;
        result = QTask_HandleInput(context, kind_1, 0);
    }
    else
    {
        QHash_ValueSet();
        kind = g_a8454c;
        result = QTask_HandleInput(context, kind, 0);
        Sys_DispatchOpcode_2();
    }

    result->sprite[0x9c] = arg3;
    result->sprite[0x9d] = *(&arg3 + 1);
    result->sprite[0x9e] = *(&arg3 + 2);
    result->sprite[0x9f] = *(&arg3 + 3);
    memcpy(&result->sprite[0x3c], arg2, 0x18);
    unsigned int data_4 = Script_ParseEnumReal32_8();
    int data_5 = 0;
    data_5 = result->head[7];
    int data_6 = *(((data_4 * 0x180 + data_5) << 2) + &g_a96ae0);

    if (data_6)
        data_6(result, 0);

    return result;
}
