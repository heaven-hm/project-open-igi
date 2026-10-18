// IGI native provenance: Weapon_EjectShell_9 - 0x0047EAD0 - CFG score 96.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047EAD0-Weapon_EjectShell_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Weapon_EjectShell_9.h"
struct status_msg* Weapon_EjectShell_9(int context, int arg2, int* arg3, int arg4, short arg5, int arg6, int arg7)

{
    unsigned int temp_2 = g_5c1160 + 0x18;
    int item_10 = 0x400eb90c;
    int item_24;
    strncpy(&item_24, "33KA33KA33KA", 0x14);
    void* data_1 = Level_StartMission_4();
    int item_18;
    struct status_msg* result = Script_FormatString_30(Script_FormatString(), data_1, temp_2, &item_18, &item_24, arg3,
        0x49480000, arg2, 0, arg4, 0, 1, 0);
    Script_ParseEnumReal32_31(result, 1);
    int item_c = 0;
    int item_8 = 0;
    int item_4 = 0x3f800000;
    float item_38_2 = Sys_AllocateMemory_14(Level_SetMission_5()) * 6.28318548f;
    Script_SpawnNewTask_31(result, context + 0x28, &item_c, arg7,
        (0.5f - Sys_AllocateMemory_14(Level_SetMission_5())) * 0.0188495573f, item_38_2, 0x3f800000, arg5,
        arg6);
    return result;
}
