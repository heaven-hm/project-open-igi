// IGI native provenance: HumanAI_EventQueued - 0x00450520 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00450520-HumanAI_EventQueued.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_EventQueued.hpp"
int* HumanAI_EventQueued(void* context, int* arg2)

{
    int* result = *(context + 0x810);

    if (!*result || !result)
        return 0;

    HeapList_Remove(result);
    result[1] = 0;
    *result = 0;
    result[2] = *arg2;
    result[3] = arg2[1];
    int source_val = arg2[4];

    if (source_val)
        memcpy(&result[4], source_val, 0x18);

    void* data_1 = arg2[6];

    if (!data_1)
        result[0xb] = 0;
    else
        result[0xb] = QTask_LinkEventHandler_2(data_1);

    void* flagP1 = arg2[5];

    if (!flagP1)
        result[0xa] = 0;
    else
        result[0xa] = QTask_LinkEventHandler_2(flagP1);

    *(context + 0x8e0) += 1;
    return result;
}
