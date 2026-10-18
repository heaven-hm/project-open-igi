// IGI native provenance: DynObj_TryingToRemoveUnrigi - 0x004C17C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C17C0-DynObj_TryingToRemoveUnrigi.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C17C0-DynObj_TryingToRemoveUnrigi.hpp"
int DynObj_TryingToRemoveUnrigi(int* context, int arg2)

{
    int result = 0;
    int entity_val = *context;

    if (entity_val > 0)
    {
        int* temp_1 = &context[2];

        do
        {
            if (*temp_1 == arg2)
            {
                *context = entity_val - 1;
                context[result + 2] = context[entity_val + 1];
                return result;
            }

            result += 1;
            temp_1 = &temp_1[1];
        } while (result < entity_val);
    }

    Error_Show("Trying to remove unrigistered QTask from QTaskList");

    while (true)
        /* nop */
}
