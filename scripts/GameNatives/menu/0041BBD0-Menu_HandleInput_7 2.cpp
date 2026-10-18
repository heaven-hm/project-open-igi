// IGI native provenance: Menu_HandleInput_7 - 0x0041BBD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041BBD0-Menu_HandleInput_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_HandleInput_7.h"
int* Menu_HandleInput_7(void* context)

{
    int i_1 = 0x40;
    int* result = context + 0x54;
    int i;

    do
    {
        result[1] = 0;
        *result = 0;
        result = &result[0x12];
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    *(context + 0x1254) = 0;
    *(context + 0x129c) = 0;
    *(context + 0x12a0) = 0;
    return result;
}
