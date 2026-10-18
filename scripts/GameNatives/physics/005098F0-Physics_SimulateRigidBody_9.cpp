// IGI native provenance: Physics_SimulateRigidBody_9 - 0x005098F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005098F0-Physics_SimulateRigidBody_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_SimulateRigidBody_9.hpp"
int Physics_SimulateRigidBody_9(FILETIME* context, FILETIME* arg2)

{
    return CompareFileTime(context, arg2);
}
