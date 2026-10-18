// IGI native provenance: DefineWeaponType - 0x00413F90 - CFG score 99.4% (PASS; structural only); runtime behavior not verified.
// Evidence: 00413F90-DefineWeaponType.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00413F90-DefineWeaponType.hpp"
int DefineWeaponType(long double context , long double arg2 , long double arg3 , long double arg4 , long double arg5 , long double arg6 , long double arg7 , long double arg8 , int arg9, float arg10 )

// Sig: unknown DefineWeaponType(unknown param1, unknown param2)
// Note: Registered at 0x413E52 via registrar 0x4B8890 (push 0x413F90 +
// 'DefineWeaponType'), argc bounds (0, -1). Large (2165-byte) config parser;
// name corroborated by semantics consistent with cataloged WeaponType_Open /
// *ParseConfigFile family.  Verified: r2 forced-analysis confirmed exact start
// (size 2165); P3 byte-level name<->address binding at registrar callsite
// 0x413E52 via 0x4B8890; P6 SHA-256(first 32 code
// bytes)=55f13a150373436d3a68f7b8756852c048244c7749999f8ba7bfdccdb9cef313;
// re-agent reverser/checker loop ran on this native (C decompilation pending
// ARM64 native port).
{
    int saved_obj = 0;
    float item_b0 = arg10;
    int saved_base_3 = Script_GetArg(item_b0, saved_obj);
    int entity_val = g_57b18c;
    *(entity_val * 0x6e8 + &g_56e258) = saved_base_3;
    int temp_4 = 0;

    if (entity_val > 0)
    {
        int* source_1 = &g_56e258;

        while (true)
        {
            if (saved_base_3 == *source_1)
            {
                int saved_base_2 = saved_base_3;
                item_b0 = "Weapon ID %d not unique";
                Error_Show(item_b0);

                while (true)
                    /* nop */
            }
            else
            {
                temp_4 += 1;
                source_1 = &source_1[0x1ba];

                if (temp_4 >= entity_val)
                    break;
            }
        }
    }

    if (g_5391b4 < saved_base_3)
        g_5391b4 = saved_base_3;

    item_b0 = 1.79366203e-43.0f;
    void item_80;
    Sys_ResolveSymbol_23(arg10, 1, &item_80, item_b0);
    TaskType_Set(&item_80, *(g_57b18c * 0x6e8 + &g_56e258));
    void* dest_val = &item_80;
    int i = 0xffffffff;
    int temp_8 = g_57b18c * 0x6e8;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_6 = ~i;
    int source_3;
    int dest_3;
    dest_3 = memcpy(temp_8 + 0x56e268, dest_val - entity_6, entity_6 & 0xfffffffc);
    memcpy(dest_3, source_3, entity_6 & 3);
    Sys_ResolveSymbol_23(arg10, 2, temp_8 + 0x56e368, 0x100);
    Sys_ResolveSymbol_23(arg10, 3, g_57b18c * 0x6e8 + &g_56e568, 0x100);
    Sys_ResolveSymbol_23(arg10, 4, g_57b18c * 0x6e8 + &g_56e468, 0x100);
    int data_8 = g_57b18c;
    item_b0 = 7.00649232e-45.0f;
    *(data_8 * 0x6e8 + &g_56e934) = 1;
    *(g_57b18c * 0x6e8 + &g_56e868) = Script_GetArg(arg10, item_b0);
    *(g_57b18c * 0x6e8 + &g_56e86c) = Script_GetArg(arg10, 6);
    *(g_57b18c * 0x6e8 + &g_56e870) = Script_GetArg(arg10, 7);
    *(g_57b18c * 0x6e8 + &g_56e25c) = Script_GetArg(arg10, 8);
    *(g_57b18c * 0x6e8 + &g_56e894) = Script_GetArg(arg10, 9);
    Profile_ValueSet(arg10, 1.40129846e-44.0f);
    *(g_57b18c * 0x6e8 + &g_56e8ac) = context;
    Profile_ValueSet(arg10, 1.54142831e-44.0f);
    *(g_57b18c * 0x6e8 + &g_56e8b0) = arg2;
    Profile_ValueSet(arg10, 1.68155816e-44.0f);
    int data_17 = g_57b18c;
    item_b0 = 1.821688e-44.0f;
    *(data_17 * 0x6e8 + &g_56e8b4) = arg3;
    Profile_ValueSet(arg10, item_b0);
    *(g_57b18c * 0x6e8 + &g_56e8b8) = arg4;
    *(g_57b18c * 0x6e8 + &g_56e898) = Script_GetArg(arg10, 0xe);
    *(g_57b18c * 0x6e8 + &g_56e89c) = Script_GetArg(arg10, 0xf);
    *(g_57b18c * 0x6e8 + &g_56e8a0) = Script_GetArg(arg10, 0x10);
    *(g_57b18c * 0x6e8 + &g_56e8a4) = Script_GetArg(arg10, 0x11);
    Profile_ValueSet(arg10, 2.52233724e-44.0f);
    *(g_57b18c * 0x6e8 + &g_56e8bc) = arg5;
    Profile_ValueSet(arg10, 2.66246708e-44.0f);
    *(g_57b18c * 0x6e8 + &g_56e8c0) = arg6;
    Profile_ValueSet(arg10, 2.80259693e-44.0f);
    int data_28 = g_57b18c;
    item_b0 = 2.94272678e-44.0f;
    *(data_28 * 0x6e8 + &g_56e8c4) = arg7;
    Profile_ValueSet(arg10, item_b0);
    *(g_57b18c * 0x6e8 + &g_56e8c8) = arg8;
    Profile_ValueSet(arg10, 3.08285662e-44.0f);
    int data_32 = g_57b18c;
    *(data_32 * 0x6e8 + &g_56e8cc) = context;
    int source_5 = data_32 * 0x6e8;
    Profile_ValueSet(arg10, 3.22298647e-44.0f);
    *(source_5 + 0x56e8d0) = arg2;
    Sys_ResolveSymbol_23(arg10, 0x18, source_5 + 0x56e668, 0x100);
    int source_7 = g_57b18c * 0x6e8;
    Profile_ValueSet(arg10, 3.50324616e-44.0f);
    *(source_7 + 0x56e8a8) = arg3;
    Sys_ResolveSymbol_23(arg10, 0x1a, source_7 + 0x56e768, 0x100);
    item_b0 = 3.78350585e-44.0f;
    int data_35 = Script_GetArg(arg10, item_b0);
    int entity_59 = g_57b18c;
    *(entity_59 * 0x6e8 + &g_56e260) = data_35;
    int source_9 = entity_59 * 0x6e8;
    *(source_9 + 0x56e264) = Script_GetArg(arg10, 0x1c);
    Sys_ResolveSymbol_23(arg10, 0x1d, source_9 + 0x56e874, 0x10);
    Sys_ResolveSymbol_23(arg10, 0x1e, g_57b18c * 0x6e8 + &g_56e884, 0x10);
    *(g_57b18c * 0x6e8 + &g_56e8d4) = Script_GetArg(arg10, 0x1f);
    int data_43 = Script_GetArg(arg10, 0x20);
    int entity_65 = g_57b18c;
    item_b0 = 4.62428493e-44.0f;
    *(entity_65 * 0x6e8 + &g_56e8d8) = data_43;
    *(g_57b18c * 0x6e8 + &g_56e8dc) = Script_GetArg(arg10, item_b0);
    *(g_57b18c * 0x6e8 + &g_56e8e0) = Script_GetArg(arg10, 0x22);
    *(g_57b18c * 0x6e8 + &g_56e8e4) = Script_GetArg(arg10, 0x23);
    *(g_57b18c * 0x6e8 + &g_56e8e8) = Script_GetArg(arg10, 0x24);
    *(g_57b18c * 0x6e8 + &g_56e8ec) = Script_GetArg(arg10, 0x25);
    *(g_57b18c * 0x6e8 + &g_56e8f0) = Script_GetArg(arg10, 0x26);
    *(g_57b18c * 0x6e8 + &g_56e8f8) = Script_GetArg(arg10, 0x27);
    int data_51 = Script_GetArg(arg10, 0x28);
    int entity_81 = g_57b18c;
    item_b0 = 5.7453237e-44.0f;
    *(entity_81 * 0x6e8 + &g_56e8fc) = data_51;
    *(g_57b18c * 0x6e8 + &g_56e8f4) = Script_GetArg(arg10, item_b0);
    int data_53 = Script_GetArg(arg10, 0x2a);
    int entity_85 = g_57b18c;
    *(entity_85 * 0x6e8 + &g_56e900) = data_53;
    int source_11 = entity_85 * 0x6e8;
    *(source_11 + 0x56e904) = Script_GetArg(arg10, 0x2b);
    Sys_ResolveSymbol_23(arg10, 0x2c, source_11 + 0x56e908, 0x10);
    Sys_ResolveSymbol_23(arg10, 0x2d, g_57b18c * 0x6e8 + &g_56e918, 0x10);
    Profile_ValueSet(arg10, 6.44597294e-44.0f);
    int data_60 = g_57b18c;
    float dest_4 = 6.72623263e-44.0f;
    item_b0 = 6.58610278e-44.0f;
    *(data_60 * 0x6e8 + &g_56e928) = arg4;
    int source_13 = data_60 * 0x6e8;
    *(source_13 + 0x56e92c) = Script_GetArg(arg10, item_b0);
    *(source_13 + &g_56e938) = 0;
    int item_9c;
    void* item_98;

    if (*(source_13 + 0x56e92c) != 0x181)
    {
        int entity_93 = 0;
        entity_93 = *(source_13 + 0x56e92c);
        int data_68 = *(((*g_5391ac * 0x180 + entity_93) << 2) + &g_a96ae0);

        if (data_68)
        {
            item_b0 = &item_9c;
            data_68(0, item_b0);
            void* data_70 = Mem_Alloc(item_9c, 4);
            int entity_94 = g_57b18c;
            item_98 = data_70;
            float item_94_1 = arg10;
            float item_90_1 = 6.72623263e-44.0f;
            int entity_96 = entity_94 * 0x6e8;
            *(entity_96 + &g_56e938) = data_70;
            int data_71 = 0;
            data_71 = *(entity_96 + 0x56e92c);
            int data_72 = 0;
            data_72 = g_5391ad;
            (*(((data_71 + data_72 * 0x180) << 2) + &g_a96ae0))(0, &item_98);
            dest_4 = item_90_1;
        }
    }

    int source_14 = g_57b18c;
    item_b0 = dest_4;
    int source_16 = source_14 * 0x6e8;
    *(source_16 + 0x56e930) = Script_GetArg(arg10, item_b0);
    *(source_16 + 0x56e93c) = 0;

    if (*(source_16 + 0x56e930) != 0x181)
    {
        int data_76 = 0;
        int entity_100 = 0;
        data_76 = g_5391af;
        entity_100 = *(source_16 + 0x56e930);
        int data_80 = *(((data_76 * 0x180 + entity_100) << 2) + &g_a96ae0);

        if (data_80)
        {
            int* saved_base_1 = &item_9c;
            item_b0 = 0.0f;
            data_80(item_b0, saved_base_1);
            void* data_82 = Mem_Alloc(item_9c, 4);
            int entity_101 = g_57b18c;
            item_98 = data_82;
            float item_94_2 = arg10;
            int entity_103 = entity_101 * 0x6e8;
            *(entity_103 + 0x56e93c) = data_82;
            int data_83 = 0;
            data_83 = *(entity_103 + 0x56e930);
            int item_90_2 = dest_4 + 1;
            (*(((data_83 + *g_5391b0 * 0x180) << 2) + &g_a96ae0))(0, &item_98);
        }
    }

    g_57b18c += 1;
    item_98 = 1;
    int item_90_3 = 0;
    int item_8c = 0x3ff00000;
    char* item_88 = &g_567c74;
    memcpy(arg9, &item_98, 0x18);
    return arg9;
}
