// IGI native provenance: Menu_LeaveMenus_22 - 0x0042DB00 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0042DB00-Menu_LeaveMenus_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_LeaveMenus_22.hpp"
char* Menu_LeaveMenus_22(int context, int* arg2)

{
    int* source_val = arg2;
    void* flagP = *source_val;
    float dest_val = source_val[1];
    (*(((SoundSys_Update3DPosition_18() * 0x180 + Script_ExecuteLevelScript_16()) << 2) + &g_a96ae0))(context, source_val);
    int data_5 = source_val[2];
    source_val[2] = data_5 + 1;
    int data_6 = Script_GetArg(dest_val, data_5);
    *(flagP + 0x118) = data_6;
    arg2 = nullptr;

    if (data_6 > 0)
    {
        char* context_3 = flagP + 0x11d;
        bool flag_2_1;

        do
        {
            int data_7 = source_val[2];
            source_val[2] = data_7 + 1;
            context_3[0xffffffff] = Script_GetArg(dest_val, data_7);
            int data_9 = source_val[2];
            source_val[2] = data_9 + 1;
            *context_3 = Script_GetArg(dest_val, data_9);
            float data_11 = source_val[2];
            source_val[2] = data_11 + 1;
            Profile_ValueSet(dest_val, data_11);
🚫🚫🚫            *(context_3 + 3) =
            /* unimplemented  {fstp dword [context_val+0x3], fp0} */
            int data_12 = source_val[2];
            source_val[2] = data_12 + 1;
            *(context_3 + 7) = Script_GetArg(dest_val, data_12);
            float data_14 = source_val[2];
            source_val[2] = data_14 + 1;
            Profile_ValueSet(dest_val, data_14);
🚫🚫            /* unimplemented  {fmul fp0, dword [&g_5333b8]} */
🚫🚫🚫            *(context_3 + 0xb) =
            /* unimplemented  {fstp dword [context_val+0xb], fp0} */
            float data_15 = source_val[2];
            source_val[2] = data_15 + 1;
            Profile_ValueSet(dest_val, data_15);
🚫🚫🚫            *(context_3 + 0xf) =
            /* unimplemented  {fstp dword [context_val+0xf], fp0} */
            short idx = idx + 3;
            context_3 = &context_3[0x14];
            flag_2_1 = arg2 + 1 < *(flagP + 0x118);
            arg2 += 1;
        } while (flag_2_1);
    }

    float data_18 = source_val[2];
    source_val[2] = data_18 + 1;
    Profile_ValueSet(dest_val, data_18);
🚫🚫    /* unimplemented  {fmul fp0, dword [&g_5333b8]} */
🚫🚫🚫    *(flagP + 0x25c) =
    /* unimplemented  {fstp dword [flagP+0x25c], fp0} */
    int data_19 = source_val[2];
    source_val[2] = data_19 + 1;
    *(flagP + 0x260) = Script_GetArg(dest_val, data_19);
    int data_21 = source_val[2];
    source_val[2] = data_21 + 1;
    *(flagP + 0x261) = Script_GetArg(dest_val, data_21);
    int data_23 = source_val[2];
    source_val[2] = data_23 + 1;
    Sys_ResolveSymbol_23(dest_val, data_23, flagP + 0x262, 0x20);
    int data_24 = source_val[2];
    source_val[2] = data_24 + 1;
    int data_25 = Script_GetArg(dest_val, data_24);
    int i = 0;
    *(flagP + 0x2ac) = data_25;

    if (data_25 > 0)
    {
        arg2 = flagP + 0x284;

        do
        {
            int data_27 = source_val[2];
            source_val[2] = data_27 + 1;
            i += 1;
            *arg2 = Script_GetArg(dest_val, data_27);
            arg2 = &arg2[1];
        } while (i < *(flagP + 0x2ac));
    }

    int data_30 = source_val[2];
    source_val[2] = data_30 + 1;
    return Sys_ResolveSymbol_23(dest_val, data_30, flagP + 0x2b0, 0x10);
}
