// IGI native provenance: AI_CombatTarget_2 - 0x00450FF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00450FF0-AI_CombatTarget_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CombatTarget_2.hpp"
int* AI_CombatTarget_2(void* context, int* arg2)

{
    int data_val = arg2[8];

    if (data_val)
        data_val(context, arg2);

    int* result = HeapList_Remove(arg2);
    arg2[0xb] = 0;
    int* source_1 = arg2[0xa];

    if (!source_1)
        return result;

    return AI_CheckHearing_2(context, source_1);
}
