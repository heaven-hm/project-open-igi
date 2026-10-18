// IGI native provenance: AI_PatrolDelayStandAction - 0x0048E8C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E8C0-AI_PatrolDelayStandAction.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_PatrolDelayStandAction.h"

int* AI_PatrolDelayStandAction(int self, void* args)

{
    void* base_ptr = *(args + 0x28);
    AI_AlertAllies_3(self);
    *(args + 0x18) = Patrol_Tick;
    *(args + 0x20) = 0x48eb30;
    int result_val = *(self + 0x1514);
    *(self + 0x1514) = result_val + 1;
    return PatrolCursor_Start(self + result_val * 0x1c + 0x1518, *(base_ptr + 8), *(base_ptr + 0xc));
}
