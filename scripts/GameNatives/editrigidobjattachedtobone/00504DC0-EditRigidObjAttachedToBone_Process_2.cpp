// IGI native provenance: EditRigidObjAttachedToBone_Process_2 - 0x00504DC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00504DC0-EditRigidObjAttachedToBone_Process_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "EditRigidObjAttachedToBone_Process_2.hpp"
void* EditRigidObjAttachedToBone_Process_2(int* context, int* arg2)

{
    arg2[1] = 0;
    *arg2 = 0;
    return HeapList_Insert(*context, arg2);
}
