// IGI native provenance: Script_ExecuteLevelScript_23 - 0x004F9720 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9720-Script_ExecuteLevelScript_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F9720-Script_ExecuteLevelScript_23.hpp"
void* Script_ExecuteLevelScript_23(void* context, int arg2)

{
    if (!*(context + 0x6c) || arg2 >= *(context + 0x58) || arg2 < 0 || arg2 == 0xffffffff)
        return 0;

    int* data_4 = Script_UndefinedGraphnodeda(context) * arg2 + *(context + 0x6c);
    int data_6 = *data_4 + 1;
    int data_7 = -(data_6);
    return (data_7 - data_7) & data_4;
}
