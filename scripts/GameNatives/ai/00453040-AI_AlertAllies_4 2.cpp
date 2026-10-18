// IGI native provenance: AI_AlertAllies_4 - 0x00453040 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00453040-AI_AlertAllies_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AlertAllies_4.h"
int AI_AlertAllies_4(int* context, int* arg2, void* arg3, int arg4)

{
    *context = 0;
    *arg2 = 0;
    void* data_1 = Script_ExecuteLevelScript_23(arg3, arg4);
    *context = *(data_1 + 0x28) * 0.5f;
    int result = *(data_1 + 0x28) * 0.5f;
    *arg2 = result;
    return result;
}
