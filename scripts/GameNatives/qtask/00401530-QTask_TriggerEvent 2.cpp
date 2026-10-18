// IGI native provenance: QTask_TriggerEvent - 0x00401530 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401530-QTask_TriggerEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "QTask_TriggerEvent.h"
int QTask_TriggerEvent(short context, int arg2, int arg3)

{
    int entity_val = g_567c70 + 1;
    g_536148 += 1;
    g_567c70 = entity_val;
    int source_1 = arg2 * 0x180;
    int data_2 = context + source_1;
    *((data_2 << 2) + &g_a96ae0) = arg3;
    *(data_2 + 0x54ff34) = entity_val == 1;
    int context_val = 0;

    for (short* i = &g_af5ee8; i < 0xafa6e8; )
    {
        if (*(i + 0x1d) && *i == context && !*(source_1 + context_val + 0x54ff34))
            QTask_TriggerEvent(context_val, arg2, arg3);

        i = &i[0x18];
        context_val += 1;
    }

    QTask_DeleteTask();
    int result = g_567c70 - 1;
    g_567c70 = result;
    return result;
}
