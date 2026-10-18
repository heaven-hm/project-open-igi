// IGI native provenance: AI_CheckVisibility_21 - 0x00463700 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00463700-AI_CheckVisibility_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckVisibility_21.h"
int* AI_CheckVisibility_21(void* context, int* arg2)

{
    int* data_val = *(context + 0x7b0);

    if (data_val)
    {
        Qtask_UpdateList(data_val);
        *(context + 0x7b0) = 0;
    }

    int* result = arg2;

    if (result)
    {
        result = QTask_LinkEventHandler_2(result);
        *(context + 0x7b0) = result;
    }

    return result;
}
