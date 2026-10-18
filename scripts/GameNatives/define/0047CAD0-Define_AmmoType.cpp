// IGI native provenance: Define_AmmoType - 0x0047CAD0 - CFG score 98.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047CAD0-Define_AmmoType.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0047CAD0-Define_AmmoType.hpp"
int Define_AmmoType(long double context , int arg2, float arg3 )

// Sig: void Define_AmmoType(...)
// Note: Defines an AMMO TYPE inside an ammo definition block of a .qsc/.cfg
// script (515-byte parser body; igi.exe embeds str 'DefineAmmoType'). Args
// follow the Define-ammo grammar (name, damage, range, clip...) - not fully
// mapped, treat as parser-internal. Do NOT call outside its define block.
{
    int data_val = Script_GetArg(arg3, 0);
    int entity_val = g_5c10e4;
    char* item_ac = 0x80;
    void item_80;
    char* item_b0 = &item_80;
    *(entity_val * 0x168 + &g_5be3e0) = data_val;
    Sys_ResolveSymbol_23(arg3, 1, item_b0, item_ac);
    TaskType_Set(&item_80, *(g_5c10e4 * 0x168 + &g_5be3e0));
    Sys_ResolveSymbol_23(arg3, 2, g_5c10e4 * 0x168 + &g_5be408, 0x20);
    Sys_ResolveSymbol_23(arg3, 3, g_5c10e4 * 0x168 + &g_5be428, 0x100);
    int data_10 = Script_GetArg(arg3, 4);
    int entity_5 = g_5c10e4;
    float item_ac_1 = 7.00649232e-45.0f;
    item_b0 = arg3;
    *(entity_5 * 0x168 + &g_5be3e4) = data_10;
    Profile_ValueSet(item_b0, item_ac_1);
    *(g_5c10e4 * 0x168 + &g_5be404) = context;
    *(g_5c10e4 * 0x168 + &g_5be3e8) = Script_GetArg(arg3, 6);
    *(g_5c10e4 * 0x168 + &g_5be3ec) = Script_GetArg(arg3, 7);
    *(g_5c10e4 * 0x168 + &g_5be3f0) = Script_GetArg(arg3, 8);
    *(g_5c10e4 * 0x168 + &g_5be3f4) = Script_GetArg(arg3, 9);
    *(g_5c10e4 * 0x168 + &g_5be3f8) = Script_GetArg(arg3, 0xa);
    int data_18 = Script_GetArg(arg3, 0xb);
    int entity_21 = g_5c10e4;
    int item_a4_2 = 0xc;
    *(entity_21 * 0x168 + &g_5be3fc) = data_18;
    int dest_1 = entity_21 * 0x168;
    *(dest_1 + 0x5be400) = Script_GetArg(arg3, item_a4_2);
    char* item_ac_2 = 0x10;
    item_b0 = dest_1 + 0x5be528;
    Sys_ResolveSymbol_23(arg3, 0xd, item_b0, item_ac_2);
    Sys_ResolveSymbol_23(arg3, 0xe, g_5c10e4 * 0x168 + &g_5be538, 0x10);
    g_5c10e4 += 1;
    int item_98 = 1;
    int item_90 = 0;
    int item_8c = 0x3ff00000;
    char* item_88 = &g_567c74;
    memcpy(arg2, &item_98, 0x18);
    return arg2;
}
