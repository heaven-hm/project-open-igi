// IGI native provenance: Physics_InterpolateWaypoint_8 - 0x00509390 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00509390-Physics_InterpolateWaypoint_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_InterpolateWaypoint_8.h"
int Physics_InterpolateWaypoint_8(void* context, int arg2, int arg3)

{
    return *(context + 0xb8) + ((*(context + 0x58) * arg3 + arg2) << 3);
}
