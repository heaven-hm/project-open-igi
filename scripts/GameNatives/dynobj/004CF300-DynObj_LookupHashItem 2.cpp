// IGI native provenance: DynObj_LookupHashItem - 0x004CF300 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CF300-DynObj_LookupHashItem.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CF300-DynObj_LookupHashItem.hpp"
int* DynObj_LookupHashItem(int* context, int arg2)

{
    int temp_val = 0;
    int* result = *context;
    int* i = *result;

    while (i)
    {
        if (temp_val == arg2)
            return result;

        result = i;
        i = *i;
        temp_val += 1;
    }

    return nullptr;
}
