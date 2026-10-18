// IGI native provenance: AIGraph_Cleanup - 0x005092A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005092A0-AIGraph_Cleanup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "005092A0-AIGraph_Cleanup.hpp"
char* AIGraph_Cleanup(int* context)

{
    void* source_val = context[1];
    Physics_SolveConstraints_7(source_val);
    char* result = *(source_val + 0xb8);

    if (!result)
        return result;

    int source_1 = *(source_val + 0x58);
    return Script_ParseEnumString_24(context, result, (source_1 * source_1) << 3);
}
