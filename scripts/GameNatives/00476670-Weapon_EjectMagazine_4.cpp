// IGI native provenance: Weapon_EjectMagazine_4 - 0x00476670 - CFG score 92.6% (PASS; structural only); runtime behavior not verified.
// Evidence: 00476670-Weapon_EjectMagazine_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00476670-Weapon_EjectMagazine_4.hpp"
struct status_msg* Weapon_EjectMagazine_4(void* context, void* arg2, void* arg3, int arg4, int arg5, int* arg6, char* arg7, int arg8, int arg9)

{
    int kind = Qhash_Init(1);
    kind = g_5be3b0;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);

    if (arg2)
    {
        int* data_1 = QTask_LinkEventHandler_2(arg2);
        result->sprite[0x60] = data_1;
        result->sprite[0x61] = *(&data_1 + 1);
        result->sprite[0x62] = *(&data_1 + 2);
        result->sprite[0x63] = *(&data_1 + 3);
    }

    if (arg3)
    {
        int* data_3 = QTask_LinkEventHandler_2(arg3);
        result->sprite[0x64] = data_3;
        result->sprite[0x65] = *(&data_3 + 1);
        result->sprite[0x66] = *(&data_3 + 2);
        result->sprite[0x67] = *(&data_3 + 3);
    }

    memcpy(&result->sprite[0x3c], arg4, 0x18);
    memcpy(&result->head[0x1c], arg5, 0x28);
    int entity_1 = *arg6;
    result->sprite[0x54] = entity_1;
    result->sprite[0x55] = *(&entity_1 + 1);
    result->sprite[0x56] = *(&entity_1 + 2);
    result->sprite[0x57] = *(&entity_1 + 3);
    int entity_2 = arg6[1];
    result->sprite[0x58] = entity_2;
    result->sprite[0x59] = *(&entity_2 + 1);
    result->sprite[0x5a] = *(&entity_2 + 2);
    result->sprite[0x5b] = *(&entity_2 + 3);
    int data_5 = arg6[2];
    result->sprite[0x5c] = data_5;
    result->sprite[0x5d] = *(&data_5 + 1);
    result->sprite[0x5e] = *(&data_5 + 2);
    result->sprite[0x5f] = *(&data_5 + 3);
    DynObj_CreateMagicObject_5(result, DynObj_Missing(arg7));
    result->sprite[0x6c] = arg8;
    result->sprite[0x6d] = *(&arg8 + 1);
    result->sprite[0x6e] = *(&arg8 + 2);
    result->sprite[0x6f] = *(&arg8 + 3);
    result->sprite[0x70] = arg9;
    result->sprite[0x71] = *(&arg9 + 1);
    result->sprite[0x72] = *(&arg9 + 2);
    result->sprite[0x73] = *(&arg9 + 3);
    Qhash_Reset();
    return result;
}
