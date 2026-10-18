// IGI native provenance: Camera_ApplyLineFilter_33 - 0x004AF910 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AF910-Camera_ApplyLineFilter_33.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AF910-Camera_ApplyLineFilter_33.hpp"
int* Camera_ApplyLineFilter_33(int* context, int* arg2)

{
    int temp_val = *context;
    arg2[1] = context;
    *arg2 = temp_val;
    *(*context + 4) = arg2;
    *context = arg2;
    return context;
}
