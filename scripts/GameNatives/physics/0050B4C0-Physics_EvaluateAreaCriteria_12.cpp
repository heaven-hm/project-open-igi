// IGI native provenance: Physics_EvaluateAreaCriteria_12 - 0x0050B4C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0050B4C0-Physics_EvaluateAreaCriteria_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_EvaluateAreaCriteria_12.hpp"
int Physics_EvaluateAreaCriteria_12(int context, int arg2)

{
    int data_val = context;

    if (data_val >= arg2)
        data_val = arg2;

    int eflags;
    int data_1;
    data_1 = __bsr_gprv_memv(data_val);
    int entity_1 = 0;
    entity_1 = data_1 <= 0;
    return ((entity_1 - 1) & data_1) + 1;
}
