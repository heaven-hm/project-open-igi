// IGI native provenance: DoorCabinet_Invoke13 - 0x004423C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004423C0-DoorCabinet_Invoke13.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004423C0-DoorCabinet_Invoke13.hpp"
int DoorCabinet_Invoke13(void* arg1, char* arg2)

{
    int result = *(arg1 + 0x25d0);

    if (*(arg2 + 8) != result && *(arg2 + 0xc) != result)
    {
        *arg2 = 1;
        return result;
    }

    *arg2 = 0;
    return result;
}
