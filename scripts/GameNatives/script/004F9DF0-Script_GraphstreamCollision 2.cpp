// IGI native provenance: Script_GraphstreamCollision - 0x004F9DF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9DF0-Script_GraphstreamCollision.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_GraphstreamCollision.h"
void* Script_GraphstreamCollision(char* context, int arg2, int arg3)

{
    void* result = Script_ExecuteLevelScript_24(context);
    int* i_1 = g_a774a0;

    for (int* i = *i_1; i; i = *i)
    {
        if (i_1[2] == result)
        {
            char* item_8_2 = context;
            return Warning_Show("GraphStream - collision in hash for %s");
        }

        i_1 = i;
    }

    int* source_1 = g_a76d18;

    if (!*source_1)
        return result;

    source_1[2] = result;
    source_1[3] = arg2;
    source_1[4] = arg3;
    HeapList_Remove(source_1);
    source_1[1] = 0;
    *source_1 = 0;
    return HeapList_Insert(&g_a774a0, source_1);
}
