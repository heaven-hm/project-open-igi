// IGI native provenance: DoorCabinet_Invoke9_448510 - 0x00448510 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00448510-DoorCabinet_Invoke9_448510.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00448510-DoorCabinet_Invoke9_448510.hpp"
int DoorCabinet_Invoke9_448510(void* arg1, int* arg2)

{
    return FX_FireBallUpdate_9(*arg2, *(arg1 + 0x254));
}
