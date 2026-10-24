// IGI native provenance: QTask_DeleteTask_2 - 0x00401BE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401BE0-QTask_DeleteTask_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00401BE0-QTask_DeleteTask_2.hpp"
int* QTask_DeleteTask_2(void* context, short arg2)

{
    int* result = *(context + 8);

    for (int* i = *result; i; i = *i)
    {
        if (result[7] == arg2)
            return result;

        result = i;
    }

    return nullptr;
}
