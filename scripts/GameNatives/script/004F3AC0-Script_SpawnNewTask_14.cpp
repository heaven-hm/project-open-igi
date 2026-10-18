// IGI native provenance: Script_SpawnNewTask_14 - 0x004F3AC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F3AC0-Script_SpawnNewTask_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F3AC0-Script_SpawnNewTask_14.hpp"
int* Script_SpawnNewTask_14(void* context)

{
    if (Script_EvaluateCondition_4(context) >= 0)
    {
        int data_1 = Script_SpawnNewTask_6();
        int entity_1 = 0;
        entity_1 = *(context + 0x1c);
        int data_5 = *(((data_1 * 0x180 + entity_1) << 2) + &g_a96ae0);

        if (data_5)
            data_5(context);
    }

    Script_CallNativeHandler_6(context + 0x200f8);
    HeapList_Remove(context + 0x60);
    int* result = *(context + 0x20148);

    if (!result)
        return result;

    int* data_7 = *result;

    if (data_7)
        QTask_FindType(data_7);

    return Qtask_UpdateList(*(context + 0x20148));
}
