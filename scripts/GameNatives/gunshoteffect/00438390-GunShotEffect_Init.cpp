// IGI native provenance: GunShotEffect_Init - 0x00438390 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00438390-GunShotEffect_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00438390-GunShotEffect_Init.hpp"
char* GunShotEffect_Init()

{
    int item_4 = 0;
    g_57be78 = TaskType_Register("GunShotEffect", 0x20, QTask_OpenEvent_2(), 0);
    int data_2 = HumanPlayer_UpdateStamina();
    int item_8 = data_2;
    data_2 = g_57be78;
    QTask_RemoveEventCallback(data_2, item_8, GunShotEffect_LoadParameters);
    int data_3 = HumanPlayer_CheckGround();
    QTask_RemoveEventCallback(g_57be78, data_3, sub_4387b0);
    int data_4 = HumanPlayer_UpdateMotion();
    QTask_RemoveEventCallback(g_57be78, data_4, sub_46fb50);
    TaskType_Set("TASKTYPE_GUNSHOTEFFECT", *g_57be78);
    g_57be7c = Level_LoadObjects_6("concrete1.spr");
    char* result = Level_LoadObjects_6("concrete2.spr");
    g_57be80 = result;
    return result;
}
