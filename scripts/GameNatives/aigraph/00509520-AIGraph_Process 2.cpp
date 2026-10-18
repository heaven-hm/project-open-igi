// IGI native provenance: AIGraph_Process - 0x00509520 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00509520-AIGraph_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIGraph_Process.h"
int AIGraph_Process(void* context, int* arg2)

{
    if (!*(context + 0xb8))
        Physics_SolveConstraints_7(context);

    int data_1 = *(context + 0x58);
    return Script_CallNativeHandler_24(*arg2, "RouteTable", 0xa, *(context + 0xb8), (data_1 * data_1) << 3);
}
