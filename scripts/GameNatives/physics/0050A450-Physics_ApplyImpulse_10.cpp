// IGI native provenance: Physics_ApplyImpulse_10 - 0x0050A450 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0050A450-Physics_ApplyImpulse_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_ApplyImpulse_10.hpp"
unsigned int Physics_ApplyImpulse_10(void* context , char* arg2, unsigned int arg3)

{
    unsigned int result = Terrain_ConnectGraphNode_4(arg2, 1, arg3, *(context + 0x98));

    if (result < arg3)
        *(context + 0x9c) = 1;

    return result;
}
