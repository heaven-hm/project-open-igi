// IGI native provenance: Human_IsCrouched - 0x00463180 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00463180-AI_MoveToNode_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00463180-Human_IsCrouched.hpp"
void* Human_IsCrouched(void* context)

{
    void* result = context;
    result = *(result + 0x33d);
    return result;
}
