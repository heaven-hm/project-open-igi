// IGI native provenance: DisplayRender_Invoke2 - 0x00497920 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00497920-DisplayRender_Invoke2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DisplayRender_Invoke2.h"

int DisplayRender_Invoke2(int* arg1, int* arg2)

{
    return Sys_CheckHeap_2(*arg1, *arg2, arg2[1]);
}
