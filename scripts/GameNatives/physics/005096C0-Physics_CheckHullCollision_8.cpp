// IGI native provenance: Physics_CheckHullCollision_8 - 0x005096C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005096C0-Physics_CheckHullCollision_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_CheckHullCollision_8.hpp"
void* Physics_CheckHullCollision_8(void* context, int arg2, int arg3)

{
    if (!*(context + 0xb8))
        return 0;

    int* data_1 = Physics_InterpolateWaypoint_8(context, arg2, arg3);
    int data_3 = *data_1 + 1;
    int data_4 = -(data_3);
    return (data_4 - data_4) & data_1;
}
