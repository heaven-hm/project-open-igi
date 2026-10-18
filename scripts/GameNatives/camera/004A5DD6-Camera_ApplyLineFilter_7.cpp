// IGI native provenance: Camera_ApplyLineFilter_7 - 0x004A5DD6 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5DD6-Camera_ApplyLineFilter_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A5DD6-Camera_ApplyLineFilter_7.hpp"
void Camera_ApplyLineFilter_7(int context, void* arg2 )

{
    g_543f1c = *(arg2 + 8);
    g_543f18 = context;
    g_543f20 = arg2;
}
