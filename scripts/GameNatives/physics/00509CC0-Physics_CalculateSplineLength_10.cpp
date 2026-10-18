// IGI native provenance: Physics_CalculateSplineLength_10 - 0x00509CC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00509CC0-Physics_CalculateSplineLength_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_CalculateSplineLength_10.hpp"
int Physics_CalculateSplineLength_10(int context)

{
    if (!*((context << 2) + &g_a7b4e0))
        return 0;

    int* data_val = *((context << 2) + &g_a7b2e0);
    int result = 0;
    result = (*(*data_val + 0x1c))(data_val) >= 0;
    return result;
}
