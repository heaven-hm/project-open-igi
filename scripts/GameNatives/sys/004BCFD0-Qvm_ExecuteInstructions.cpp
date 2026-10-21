// IGI native provenance: Qvm_ExecuteInstructions - 0x004BCFD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BCFD0-Qvm_ExecuteInstructions.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BCFD0-Qvm_ExecuteInstructions.hpp"
int Qvm_ExecuteInstructions(void *context, void *arg2)
{
    *(unsigned int *)((char *)arg2 + 0x2c) = 0;
    void *program = *(void **)((char *)arg2 + 0x20);
    if (!program)
        return 0;
    return Qvm_RunProgram(program);
}
