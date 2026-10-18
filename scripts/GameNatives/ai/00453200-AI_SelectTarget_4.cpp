// IGI native provenance: AI_SelectTarget_4 - 0x00453200 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00453200-AI_SelectTarget_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_SelectTarget_4.hpp"
int* AI_SelectTarget_4(int* context)

{
    int* i_1 = g_58aa18;

    for (int* i = *i_1; i; i = *i)
    {
        if (i_1[2] == context[0xedb])
        {
            int* i_2 = i_1;
            return AI_HumanmonitorNoTarget(context);
        }

        i_1 = i;
    }

    int* source_val = g_57c748;

    if (!*source_val)
        return 0;

    HeapList_Remove(source_val);
    HeapList_Insert(&g_58aa18, source_val);
    int* item_18 = source_val;
    source_val[2] = context[0xedb];
    return AI_HumanmonitorNoTarget(context);
}
