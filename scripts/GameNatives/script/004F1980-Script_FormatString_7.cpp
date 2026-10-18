// IGI native provenance: Script_FormatString_7 - 0x004F1980 - CFG score 77.8% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004F1980-Script_FormatString_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F1980-Script_FormatString_7.hpp"
int* Script_FormatString_7(int* context, int arg2)

{
    int* result = Script_EvaluateCondition_7(*context + arg2);
    int entity_1 = *context << 4;
    int source_1;
    int dest_1;
    dest_1 = memcpy(result[2], context[2], entity_1 & 0xfffffffc);
    memcpy(dest_1, source_1, entity_1 & 3);
    *result = *context;
    return result;
}
