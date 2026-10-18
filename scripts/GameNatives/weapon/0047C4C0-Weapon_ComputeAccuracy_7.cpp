// IGI native provenance: Weapon_ComputeAccuracy_7 - 0x0047C4C0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0047C4C0-Weapon_ComputeAccuracy_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0047C4C0-Weapon_ComputeAccuracy_7.hpp"
void* Weapon_ComputeAccuracy_7(void* context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7)

{
    int result = 0;
    int* temp_val = context + 0x548;

    while (*temp_val)
    {
        result += 1;
        temp_val = &temp_val[0x32];

        if (result >= 5)
            return result;
    }

    int data_val = result * 5;
    *(context + (*(context + 0x9c) << 2) + 0x900) = context + data_val * 0x28 + 0x518;
    *(context + 0x9c) += 1;
    memcpy(context + data_val * 0x28 + 0x518, arg2, 0x18);
    memcpy(context + data_val * 0x28 + 0x530, arg3, 0x18);
    *(context + data_val * 0x28 + 0x550) = arg4;
    *(context + data_val * 0x28 + 0x54c) = arg7;
    *(context + data_val * 0x28 + 0x548) = 1;
    *(context + data_val * 0x28 + 0x554) = arg5;
    *(context + data_val * 0x28 + 0x558) = arg6;
    *(context + data_val * 0x28 + 0x5d8) = 0;
    return context + data_val * 0x28 + 0x518;
}
