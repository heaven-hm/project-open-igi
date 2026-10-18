// IGI native provenance: SmoothQTask_Create - 0x0051EE80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051EE80-SmoothQTask_Create.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SmoothQTask_Create.h"

void* SmoothQTask_Create(void* self)

{
    *(self + 0x20) = data_bc79f0;
    *(self + 0x24) = data_bc79e8;
    *(self + 0x28) = data_bc79ec;
    *(self + 0x2c) = data_bc79ed;
    return self;
}
