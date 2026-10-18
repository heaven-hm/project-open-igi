// IGI native provenance: GenericTBA_LoadParameters - 0x004480B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004480B0-GenericTBA_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GenericTBA_LoadParameters.h"
char GenericTBA_LoadParameters(long double context , void* arg2)

{
    if (*(arg2 + 0x238))
        (*((FX_TrailUpdate() << 2) + &g_a970e0))(arg2);

    if (*(arg2 + 0x190))
        *(arg2 + 0x240) = 0;
    else if (Script_FormatString_6(arg2 + 0x258))
    {
        Script_SpawnNewTask_7(arg2 + 0x258, arg2);
        *(arg2 + 0x240) = context;
    }

    if (*(g_bc2360 + 0x68) > *(arg2 + 0x23c) + 2)
        *(arg2 + 0x239) = 0;

    char result = *(arg2 + 0x239);
    int entity_2 = *(arg2 + 0x244);
    *(arg2 + 0x23a) = result;

    if (*(g_bc2360 + 0x68) > entity_2 + 2)
        *(arg2 + 0x242) = 0;

    return result;
}
