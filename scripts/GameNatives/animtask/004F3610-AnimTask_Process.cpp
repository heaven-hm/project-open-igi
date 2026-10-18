// IGI native provenance: AnimTask_Process - 0x004F3610 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F3610-AnimTask_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F3610-AnimTask_Process.hpp"
int AnimTask_Process(int* context )

{
    int result = Script_GetArg(context[2], context[1]);
    *context = result * 2 + 1;
    return result;
}
