// IGI native provenance: PatrolCursor_Start - 0x004537E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004537E0-PatrolCursor_Start.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PatrolCursor_Start.hpp"
int* PatrolCursor_Start(void** context, int arg2, int arg3)

{
    *context = PatrolPath_Find(arg2);
    context[2] = 0;
    context[4] = 0;
    *(context + 0x15) = 1;
    context[5] = 0;
    context[6] = 0xffffffff;
    int* data_1 = AI_CheckHearing_5(context, arg3);
    context[1] = data_1;
    int* result = PatrolPathCommand_Process(data_1);

    if (result == 6)
    {
        result = PatrolPath_OnTick(context);
        context[1] = result;
    }

    return result;
}
