// IGI native provenance: AI_CheckHearing_2 - 0x00451050 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00451050-AI_CheckHearing_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckHearing_2.hpp"
void* AI_CheckHearing_2(void* context, int* arg2)

{
    arg2[1] = 0;
    *arg2 = 0;
    return HeapList_Insert(context + 0x1508, arg2);
}
