// IGI native provenance: AI_CheckHearing_3 - 0x00452080 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00452080-AI_CheckHearing_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckHearing_3.hpp"
int* AI_CheckHearing_3(void* context)

{
    int* result = g_58a9c8;

    if (!*result)
        return 0;

    HeapList_Remove(result);
    result[1] = 0;
    *result = 0;
    HeapList_Insert(context + 0xe0, result);
    return result;
}
