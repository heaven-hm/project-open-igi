// IGI native provenance: FX_CouldNotCreateDirect_2 - 0x0048FFF0 - CFG score 97.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048FFF0-FX_CouldNotCreateDirect_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_CouldNotCreateDirect_2.h"
void* FX_CouldNotCreateDirect_2()

{
    int item_10 = 0x6f1d2b61;
    int item_14 = 0;
    int item_c = 0x11cfd5a0;
    int item_8 = 0x4544c7bf;
    int item_4 = 0x5453;
    g_5c8e28 = 0;
    g_5c8e24 = 0;

    if (DirectInputCreateA(FX_TrailUpdate_21(), 0x700, &g_5c8e28))
    {
        Error_Show("Could not create DirectInput keyboard object!");

        while (true)
            /* nop */

        return;
    }

    int* data_2 = g_5c8e28;

    if ((*(*data_2 + 0xc))(data_2, &item_14, &g_5c8e24, 0))
    {
        Error_Show("Could not create DirectInput device keyboard!");

        while (true)
            /* nop */

        return;
    }

    int* data_4 = g_5c8e24;

    if ((*(*data_4 + 0x2c))(data_4, 0x5348c8))
    {
        Error_Show("Could not set data format for keyboard!");

        while (true)
            /* nop */

        return;
    }

    int dest_val;
    int item_18 = dest_val;
    int dest_1 = **&g_5c8e24;
    int data_6 = FX_ExplosionSpawn_22();

    if (!(*(dest_1 + 0x34))(g_5c8e24, data_6, 6))
    {
        int* data_9 = g_5c8e24;

        if (!(*(*data_9 + 0x1c))(data_9))
            g_5c8e30 = 1;

        Display_SyncVSync();
        Display_SwapBuffers(0);
        memset(0xc28e80, 0, 0x100);
        return FX_FailedToRegisterWinp(sub_490120);
    }

    Error_Show("Could not set cooperative level for keyboard!");

    while (true)
        /* nop */
}
