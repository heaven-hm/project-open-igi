// IGI native provenance: AlarmControl_Init_2 - 0x004406D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004406D0-AlarmControl_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AlarmControl_Init_2.h"

int AlarmControl_Init_2(int* self )

{
    void* source_ptr = *self;
    return FX_FireBallUpdate_9(source_ptr, source_ptr + 0x494);
}
