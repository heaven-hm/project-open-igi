// IGI native provenance: AlarmLight_Trigger - 0x00439770 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00439770-AlarmLight_Trigger.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AlarmLight_Trigger.h"

int AlarmLight_Trigger(void* self, int* args)

{
    *args = self + 0x20;
    args[1] = self + 0x70;
    int result = *(self + 0x1a4);
    args[2] = result;
    return result;
}
