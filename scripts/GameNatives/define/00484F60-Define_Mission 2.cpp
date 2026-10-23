// IGI native provenance: DefineMission - 0x00484F60 - CFG score 97.6% (PASS; structural only); runtime behavior not verified.
// Evidence: 00484F60-Define_Mission.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Define_Mission.h"
int DefineMission(int context, float arg2 )

// Sig: void DefineMission(...)
// Note: Defines a MISSION entry during mission-list parsing (480-byte parser).
// Called by the engine's mission definition grammar; do NOT call from gameplay
// scripts.
{
    int data_val = Script_GetArg(arg2, 0);
    *(g_5c89fc * 0x4a8 + &g_5c1590) = data_val;

    if (!FX_ExplosionSpawn_6(data_val))
    {
        int data_4 = g_5c89fc;
        char* item_24_2 = 0x20;
        int item_2c = 1;
        float item_30 = arg2;
        Sys_ResolveSymbol_23(item_30, item_2c, data_4 * 0x4a8 + &g_5c1594, item_24_2);
        Sys_ResolveSymbol_23(arg2, 2, g_5c89fc * 0x4a8 + &g_5c15b4, 0x100);
        Sys_ResolveSymbol_23(arg2, 3, g_5c89fc * 0x4a8 + &g_5c16b4, 0x20);
        int dest_1 = g_5c89fc * 0x4a8;
        *(dest_1 + 0x5c1a34) = Script_GetArg(arg2, 4);
        Sys_ResolveSymbol_23(arg2, 5, dest_1 + 0x5c16d4, 0x100);
        int data_12 = g_5c89fc;
        char* item_24_3 = 0x100;
        int item_2c_1 = 6;
        item_30 = arg2;
        Sys_ResolveSymbol_23(item_30, item_2c_1, data_12 * 0x4a8 + &g_5c17d4, item_24_3);
        Sys_ResolveSymbol_23(arg2, 7, g_5c89fc * 0x4a8 + &g_5c18d4, 0x100);
        Sys_ResolveSymbol_23(arg2, 8, g_5c89fc * 0x4a8 + &g_5c19d4, 0x20);
        Sys_ResolveSymbol_23(arg2, 9, g_5c89fc * 0x4a8 + &g_5c19f4, 0x20);
        int data_20 = g_5c89fc;
        char* item_24_4 = 0x20;
        int item_2c_2 = 0xa;
        item_30 = arg2;
        Sys_ResolveSymbol_23(item_30, item_2c_2, data_20 * 0x4a8 + &g_5c1a14, item_24_4);
        g_5c89fc += 1;
        int item_18 = 1;
        int item_10 = 0;
        int item_c = 0x3ff00000;
        char* item_8 = &g_567c74;
        memcpy(context, &item_18, 0x18);
        return context;
    }

    int saved_index_1 = *(g_5c89fc * 0x4a8 + &g_5c1590);
    Error_Show("Mission ID %d not unique");

    while (true)
        /* nop */
}
