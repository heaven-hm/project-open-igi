// IGI native provenance: Display_SetDisplayMode_11 - 0x004966B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004966B0-Display_SetDisplayMode_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetDisplayMode_11.h"
int* Display_SetDisplayMode_11(int* context, int** arg2)

{
    int* result = context;
    void* entity_val = *result;

    if (entity_val)
        *(entity_val + 4) = result[1];

    int* entity_1 = result[1];

    if (entity_1)
        *entity_1 = *result;

    if (*arg2 == result)
    {
        result = *result;
        *arg2 = result;
    }

    return result;
}
