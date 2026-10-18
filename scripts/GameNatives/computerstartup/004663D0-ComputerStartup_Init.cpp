// IGI native provenance: ComputerStartup_Init - 0x004663D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004663D0-ComputerStartup_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004663D0-ComputerStartup_Init.hpp"
int ComputerStartup_Init()

{
    int item_4 = 0;
    g_53f520 = TaskType_Register("ComputerStartup", 0x90, QTask_OpenEvent_2(), 0);
    int data_2 = StatusScreen_GetEvent();
    int item_8 = data_2;
    data_2 = g_53f520;
    QTask_TriggerEvent(data_2, item_8, ComputerStartup_LoadParameters);
    QTask_TriggerEvent(g_53f520, 1, ComputerStartup_Setup);
    QTask_TriggerEvent(g_53f520, 2, ComputerStartup_Process);
    int data_3 = Sys_RendermodeFailedToCr_2("ComputerStartup");
    g_5bdc54 = data_3;
    Sys_CheckHeap_2(Display_CheckCapabilities_7(), data_3, ComputerStartup_Update);
    int data_5 = Display_RestoreDevice_14("ComputerStartup");
    g_5bdc50 = data_5;
    return Display_SetGammaRamp_14(data_5, sub_466c60);
}
