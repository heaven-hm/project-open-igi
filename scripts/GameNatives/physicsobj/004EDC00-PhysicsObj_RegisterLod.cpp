// IGI native provenance: PhysicsObj_RegisterLod - 0x004EDC00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EDC00-PhysicsObj_RegisterLod.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PhysicsObj_RegisterLod.hpp"

int* PhysicsObj_RegisterLod(int self)

{
    g_a5efd4 = self;
    Script_SetSymbolContext("Task_New", self);
    int* result_1 = Qvm_Load("LOCAL:lod.qsc");

    if (!result_1)
        return result_1;

    Script_Init("LOCAL:lod.qsc", 1, 0, 0);
    Qvm_Execute(result_1);
    return Qvm_Cleanup(result_1);
}
