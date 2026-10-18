// IGI native provenance: QTask_RemoveEventCallback - 0x00401400 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401400-QTask_RemoveEventCallback.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00401400-QTask_RemoveEventCallback.hpp"
int QTask_RemoveEventCallback(short context, int arg2, int arg3)

{
    int entity_val = g_567c6c + 1;
    g_536148 += 1;
    unsigned int source_3 = arg2 * 0x180;
    g_567c6c = entity_val;
    unsigned int data_4 = context + source_3;
    *((data_4 << 2) + &g_a96ae0) = arg3;
    *(data_4 + 0x54ff34) = entity_val == 1;
    int context_val = 0;

    for (short* i = &g_af5ee8; i < 0xafa6e8; )
    {
        if (*(i + 0x1d) && *i == context && !*(source_3 + context_val + 0x54ff34))
            QTask_RemoveEventCallback(context_val, arg2, arg3);

        i = &i[0x18];
        context_val += 1;
    }

    QTask_DeleteTask();
    int result = g_567c6c - 1;
    g_567c6c = result;
    return result;
}
