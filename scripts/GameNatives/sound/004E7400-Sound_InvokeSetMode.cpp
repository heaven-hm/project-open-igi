// IGI native provenance: Sound_InvokeSetMode - 0x004E7400 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E7400-Sound_InvokeSetMode.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sound_InvokeSetMode.hpp"

int Sound_InvokeSetMode(void* arg1)

{
    if (g_a5ea65)
    {
        Display_SetMode(0xa5e620);
        __builtin_memcpy(0xa5e940, 0xa5e620, 0x11c);
        g_a5ea65 = 0;
    }

    Display_SyncVSync_4();
    int* i = *(arg1 + 8);

    if (*i && i)
    {
        int afa7e0_1 = g_afa7e0;

        do
        {
            int* edx_1 = *i;
            void* eax_4;

            if (!edx_1)
                eax_4 = nullptr;
            else
            {
                int eax_1 = *edx_1;
                int eax_2 = -(eax_1);
                eax_4 = (eax_2 - eax_2) & edx_1;
            }

            *((afa7e0_1 << 2) + &g_afa6e0) = eax_4;
            g_afa7e0 = afa7e0_1 + 1;
            int eax_5 = StatusScreen_GetEvent();
            int edx_2 = 0;
            edx_2 = i[7];
            int eax_6 = *(((eax_5 * 0x180 + edx_2) << 2) + &g_a96ae0);

            if (eax_6)
                eax_6(i);

            afa7e0_1 = g_afa7e0 - 1;
            g_afa7e0 = afa7e0_1;
            i = *((afa7e0_1 << 2) + &g_afa6e0);
        } while (i);
    }

    int i_1 = 0;

    if (data_a5ea68 > 0)
    {
        int* ebx_1 = &data_a5e740;

        do
        {
            void* source_ptr = *ebx_1;
            int eax_8 = StatusScreen_GetEvent();
            int ecx_6 = 0;
            ecx_6 = *(source_ptr + 0x1c);
            (*(((eax_8 * 0x180 + ecx_6) << 2) + &g_a96ae0))(source_ptr);
            i_1 += 1;
            ebx_1 = &ebx_1[2];
        } while (i_1 < data_a5ea68);
    }

    Display_SwapBuffers_4();
    int result = g_a5ea60 + 1;
    g_a5ea60 = result;
    return result;
}
