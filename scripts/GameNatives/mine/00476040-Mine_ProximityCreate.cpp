// IGI native provenance: Mine_ProximityCreate - 0x00476040 - CFG score 85.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 00476040-Mine_ProximityCreate.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Mine_ProximityCreate.hpp"
struct status_msg* Mine_ProximityCreate(void* context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, char arg9)

// Sig: void Mine_ProximityCreate(void)
// Note: Creates a PROXIMITY MINE entity: initializes hash state via
// QHash_Init(0x4B0D60), reads mine id word 0x5BE3A0. IGI-MP hooks for network
// mine sync.
{
    int kind = Qhash_Init(1);
    kind = g_5be3a0;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    memcpy(&result->sprite[0x3c], arg2, 0x18);
    memcpy(&result->sprite[0x54], arg3, 0x28);
    int dest_2 = arg4;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_2;
        dest_2 += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_1 = ~i;
    int source_3;
    int dest_5;
    dest_5 = memcpy(&result->sprite[0xbc], dest_2 - entity_1, entity_1 & 0xfffffffc);
    memcpy(dest_5, source_3, entity_1 & 3);
    *(result + 0x240) = arg6;
    *(result + 0x238) = arg5;
    *(result + 0x244) = arg7;
    *(result + 0x249) = arg9;
    FX_FireBallUpdate_2(result, 1.0f, arg8, 0, 0x3f800000, 0, 5, 1, "explo_proxy");
    Mine_Process(result, 0);
    Qhash_Reset();
    return result;
}
