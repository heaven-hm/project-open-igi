// IGI native provenance: Light_ComputeDirectionalLight_10 - 0x0052D350 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052D350-Light_ComputeDirectionalLight_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_ComputeDirectionalLight_10.hpp"
void* Light_ComputeDirectionalLight_10(void* context, float arg2)

{
    *(context + 0x48) = arg2;
    return context;
}
