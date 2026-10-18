// IGI native provenance: Script_ParseEnumString_30 - 0x004FDCD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FDCD0-Script_ParseEnumString_30.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumString_30.h"
double* Script_ParseEnumString_30(double* context, void* arg2)

{
    *context = *(arg2 + 0xa0) + *(arg2 + 0x88);
    context[1] = *(arg2 + 0xa8) + *(arg2 + 0x90);
    context[2] = *(arg2 + 0xb0) + *(arg2 + 0x98);
    *context = *context * 0.5;
    context[1] = context[1] * 0.5;
    context[2] = context[2] * 0.5;
    return context;
}
