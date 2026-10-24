// IGI native provenance: Camera_UpdateViewCone_29 - 0x004AE5F3 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AE5F3-Camera_UpdateViewCone_29.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AE5F3-Camera_UpdateViewCone_29.hpp"
void Camera_UpdateViewCone_29(int context, int arg2, int arg3)

{
    int context_val = 0x546a98;

    if (!arg2)
        return;

    if (arg2 < 0)
    {
        arg2 = -(arg2);
        context_val = 0x546bf8;
    }

    if (!arg3)
        *context = 0;

    while (arg2)
    {
        int data_2 = arg2;
        context_val += 0x54;
        arg2 s>>= 3;
        int data_val = data_2 & 7;

        if (data_val)
        {
            int data_3 = data_val * 3;
            int* source_1 = context_val + (data_3 << 2);

            if (*(context_val + (data_3 << 2)) >= 0x8000)
            {
                int item_10 = *source_1;
                void item_c;
                int* dest_1 = &item_c;
                int* source_2 = &source_1[1];
                *dest_1 = *source_2;
                dest_1[1] = source_2[1];
                *(&item_10 + 2) -= 1;
                source_1 = &item_10;
            }

            Camera_ApplyPostProcess_29(context, source_1);
        }
    }
}
