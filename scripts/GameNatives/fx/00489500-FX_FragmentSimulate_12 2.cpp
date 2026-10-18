// IGI native provenance: FX_FragmentSimulate_12 - 0x00489500 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489500-FX_FragmentSimulate_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_FragmentSimulate_12.h"
int* FX_FragmentSimulate_12(int context, void* arg2, void* arg3, int arg4)

{
    int* result = ((arg4 * 3 + 3) << 4) + context;
    result[2] = QTask_LinkEventHandler_2(arg3);
    result[4] = 0;
    result[5] = 0;
    unsigned int data_2 = Frames_GetUpdateCounter();
    result[8] = 0;
    result[6] = data_2;
    result[9] = 0;
    result[0xa] = 0;
    *(result + 0x29) = 0;
    *(result + 0x2a) = 1;
    result[1] = 0;
    *result = 0;
    HeapList_Insert(arg2, result);
    return result;
}
