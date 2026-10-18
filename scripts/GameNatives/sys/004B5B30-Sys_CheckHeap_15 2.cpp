// IGI native provenance: Resource_CalculatePathHash - 0x004B5B30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B5B30-Resource_CalculatePathHash.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Resource_CalculatePathHash.h"
unsigned int Resource_CalculatePathHash(char* context)

{
    char* temp_val = context;
    int data_val = 1;
    char i = *temp_val;

    while (i)
    {
        data_val = (data_val + 1) * i;
        i = temp_val[1];
        temp_val = &temp_val[1];
    }

    return COMBINE(0, data_val) % 0x3037;
}
