// IGI native provenance: AI_CheckVisibility_17 - 0x0045EFD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045EFD0-AI_CheckVisibility_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckVisibility_17.h"
int* AI_CheckVisibility_17(void* context)

{
    int* data_val = *(context + 0x768);

    if (data_val)
        Qtask_UpdateList(data_val);

    int* data_1 = *(context + 0x7b0);

    if (data_1)
        Qtask_UpdateList(data_1);

    int* data_2 = *(context + 0x7b4);

    if (data_2)
        Qtask_UpdateList(data_2);

    int* data_3 = *(context + 0x838);

    if (data_3)
        Qtask_UpdateList(data_3);

    int* result = *(context + 0x83c);

    if (result)
        result = Qtask_UpdateList(result);

    int* source_1 = *(context + 0x840);

    if (!source_1)
        return result;

    return Qtask_UpdateList(source_1);
}
