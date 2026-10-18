// Retail runtime observation: Level_Load - 0x004F0D40 - 2 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Catalog alias Task_New_2; working label Level_Load follows its retail error string. Script path and rate observed.
// IGI native provenance: Task_New_2 - 0x004F0D40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0D40-Task_New_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Task_New_2.h"
int Task_New_2(int context, int arg2, int arg3, void* arg4, int arg5)

{
    int item_4 = arg3;

    for (int* i = &g_a71890; i < &g_a75890; i = &i[1])
    {
        int* data_val = *i;

        if (data_val)
        {
            Qtask_UpdateList(data_val);
            *i = 0;
        }
    }

    Script_SetSymbolContext("Task_New", g_a758a4);
    int* data_2 = Qvm_Load(arg4);

    if (data_2)
    {
        Script_Init(arg4, 1, 0, 0);
        Qvm_Execute(data_2);
        Qvm_Cleanup(data_2);
        *(&item_4 + 3) = 1;
        int data_3 = Script_ParseEnumReal32_8();
        QTask_ProcessQueue_2(g_a758a4, data_3, (&item_4 + 3));
        *(g_a758a4 + 0x24) = 0;
        *(g_a758a4 + 0x28) = 0;
        *(g_a758a4 + 0x2c) = arg5;
        int result = 0x3e8 / arg5;
        *(g_a758a4 + 0x30) = result;
        return result;
    }

    void* item_10_2 = arg4;
    Error_Show("Level_Load(): Couldn't load script: %s");

    while (true)
        /* nop */
}
