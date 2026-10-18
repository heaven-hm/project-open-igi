// IGI native provenance: KEY_ANGLE - 0x00406240 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00406240-KEY_ANGLE.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "KEY_ANGLE.h"
int KEY_ANGLE(int context, int* arg2, void** arg3)

{
    int result = context;

    if (arg3)
        *arg3 = &g_536998[result * 0x24];

    if (arg2)
    {
        result *= 9;
        *arg2 = *((result << 2) + &g_5369b8);
    }

    return result;
}
