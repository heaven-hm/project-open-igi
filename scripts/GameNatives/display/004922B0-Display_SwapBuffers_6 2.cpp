// IGI native provenance: Display_SwapBuffers_6 - 0x004922B0 - CFG score 73.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004922B0-Display_SwapBuffers_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SwapBuffers_6.h"
int __stdcall Display_SwapBuffers_6(char* context, int* arg2 , int* arg3)

{
    if (*arg3 == 1)
    {
        char* data_1 = arg3[2];
        char* dest_1 = context;
        char* data_2;
        int data_3;

        if (data_1)
        {
            data_2 = Camera_UpdateViewCone_6(dest_1, data_1);
            data_3 = -(data_2);
        }

        if (!data_1 || -((data_3 - data_3)))
        {
            int i = 0xffffffff;
            void* temp_2 = arg3[5] + *arg3[6] * 0x668;

            while (i)
            {
                bool flag_0_1 = 0 != *dest_1;
                dest_1 = &dest_1[1];
                i -= 1;

                if (!flag_0_1)
                    break;
            }

            int entity_2 = ~i;
            int source_2;
            int dest_4;
            dest_4 = memcpy(temp_2 + 0x4f8, dest_1 - entity_2, entity_2 & 0xfffffffc);
            memcpy(dest_4, source_2, entity_2 & 3);
            memcpy(temp_2 + 0x578, arg2, 0xec);
            memcpy(temp_2, &arg3[9], 0x4f8);
            *(temp_2 + 0x664) = *arg2 >> 0x13 & 1;
            int* data_12 = arg3[6];
            *data_12 += 1;

            if (arg3[2])
                return 0;
        }
    }

    return 1;
}
