// IGI native provenance: Weapon_EntityCreate - 0x00477C50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00477C50-Weapon_EntityCreate.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00477C50-Weapon_EntityCreate.hpp"
struct status_msg* Weapon_EntityCreate(void* context, short arg2, int* arg3, int arg4, char arg5)

// Sig: WeaponEntityView* Weapon_EntityCreate(unsigned int p1, unsigned short objectType,
// unsigned int weaponType, unsigned int p4, unsigned char p5)
// Note: Validates the weapon type, allocates/configures the weapon entity,
// initializes object fields, and dispatches the type handler. Ghidra/r2 CFG
// agreement: 6 basic blocks and 7 edges; see
// assets/IGI1-Native-Name-Evidence.json.
{
    Qhash_Init(1);
    int* dest_val = arg3;
    int* kind = HumanPlayer_CheckCollision_10(dest_val);

    if (!kind)
    {
        int* item_c_1 = dest_val;
        Warning_Show("Unknown weapon type: %d");
        arg3 = g_5be3c6;
        kind = arg3;
    }
    else
        kind = kind[0x1b6];

    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    int* item_18 = dest_val;
    result->sprite[0x3d] = arg5;
    result->sprite[0x70] = arg2;
    result->sprite[0x71] = *(&arg2 + 1);
    Game_DataSymbolLoad(&result->sprite[0x50], "%d");
    unsigned int data_4 = Script_ParseEnumReal32_8();
    int data_5 = 0;
    data_5 = result->head[7];
    int data_6 = *(((data_4 * 0x180 + data_5) << 2) + &g_a96ae0);

    if (data_6)
        data_6(result, 0);

    result->sprite[0x90] = arg4;
    result->sprite[0x91] = *(&arg4 + 1);
    result->sprite[0x92] = *(&arg4 + 2);
    result->sprite[0x93] = *(&arg4 + 3);
    Qhash_Reset();
    return result;
}
