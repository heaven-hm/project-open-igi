// IGI native provenance: AI_UpdateSensors_20 - 0x00463640 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00463640-AI_UpdateSensors_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_UpdateSensors_20.h"
int* AI_UpdateSensors_20(void* context, int* arg2)

{
    int* data_val = *(context + 0x768);

    if (data_val)
    {
        Qtask_UpdateList(data_val);
        *(context + 0x768) = 0;
    }

    int* result = arg2;

    if (result)
    {
        result = QTask_LinkEventHandler_2(result);
        *(context + 0x768) = result;
    }

    return result;
}
