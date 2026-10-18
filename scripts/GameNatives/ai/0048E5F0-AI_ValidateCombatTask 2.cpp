// IGI native provenance: AI_ValidateCombatTask - 0x0048E5F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E5F0-AI_ValidateCombatTask.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_ValidateCombatTask.h"

int* AI_ValidateCombatTask(void* self )

{
    void* source_ptr = *(self + 0x28);
    int* result_1 = *(source_ptr + 0x24);

    if (result_1)
        result_1 = Qtask_UpdateList(result_1);

    int* source_ptr_2 = *(source_ptr + 0x20);

    if (!source_ptr_2)
        return result_1;

    return Qtask_UpdateList(source_ptr_2);
}
