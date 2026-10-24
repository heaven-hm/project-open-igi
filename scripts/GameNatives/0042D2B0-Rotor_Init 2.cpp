// IGI native provenance: Rotor_Init - 0x0042D2B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0042D2B0-Rotor_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Rotor_Init.h"
void* Rotor_Init()

{
    int item_4 = 0;
    g_57bd10 = TaskType_Register("Rotor", 0x140, MagicObj_GetTypeId(), 0);
    int data_2 = Display_RestoreDevice_14("RotorPrimitive");
    g_57bd14 = data_2;
    Display_SetGammaRamp_14(data_2, Rotor_LoadParameters);
    QTask_RemoveEventCallback(g_57bd10, 0, Rotor_Setup);
    QTask_TriggerEvent(g_57bd10, 1, Rotor_Tick);
    QTask_TriggerEvent(g_57bd10, 2, sub_51b940);
    int data_4 = MagicObj_GetEventId();
    int item_8 = data_4;
    data_4 = g_57bd10;
    QTask_RemoveEventCallback(data_4, item_8, Rotor_Update);
    int data_5 = Level_LoadLevel_3();
    QTask_RemoveEventCallback(g_57bd10, data_5, sub_42d9b0);
    int data_6 = Level_InitTextures_3();
    QTask_RemoveEventCallback(g_57bd10, data_6, 0);
    int data_7 = SoundSys_Update3DPosition_18();
    int item_8_3 = data_7;
    data_7 = g_57bd10;
    QTask_RemoveEventCallback(data_7, item_8_3, sub_42d9f0);
    int data_8 = SoundSys_SetMasterVolume_18();
    QTask_RemoveEventCallback(g_57bd10, data_8, sub_42da60);
    QTask_RemoveEventCallback(g_57bd10, 4, Rotor_OnEvent);
    int data_9 = StatusScreen_GetEvent();
    int item_8_5 = data_9;
    data_9 = g_57bd10;
    QTask_TriggerEvent(data_9, item_8_5, 0);
    int data_10 = DynObj_FindSymbol_6();
    QTask_RemoveEventCallback(g_57bd10, data_10, 0);
    int data_11 = SoundSys_SetFrequency_18();
    QTask_RemoveEventCallback(g_57bd10, data_11, PhysicsObj_Setup);
    int data_12 = Weapon_CheckAmmo_8();
    int item_8_8 = data_12;
    data_12 = g_57bd10;
    QTask_RemoveEventCallback(data_12, item_8_8, 0);
    return TaskType_Set("TASKTYPE_ROTOR", *g_57bd10);
}
