// IGI native provenance: Computer_Create - 0x0046B940 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046B940-Computer_Create.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Computer_Create.h"

void* Computer_Create(void* self, int* args)

{
    return EditRigidObjAttachedToBone_Process_2(args, self + 0x54);
}
