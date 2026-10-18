// IGI native provenance: Script_LevelqtaskidDAlready - 0x004F1090 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F1090-Script_LevelqtaskidDAlready.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_LevelqtaskidDAlready.h"
int* Script_LevelqtaskidDAlready(int context, void* arg2)

{
    int* data_val = *((context << 2) + &g_a71890);

    if (data_val)
    {
        if (!*data_val)
            Qtask_UpdateList(data_val);
        else
        {
            int item_8_1 = context;
            Error_Show("LevelQTaskID %d already defined");

            while (true)
                /* nop */
        }
    }

    int* result = QTask_LinkEventHandler_2(arg2);
    *((context << 2) + &g_a71890) = result;
    return result;
}
