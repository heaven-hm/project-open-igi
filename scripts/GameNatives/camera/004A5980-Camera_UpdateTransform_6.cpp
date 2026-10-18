// IGI native provenance: Camera_UpdateTransform_6 - 0x004A5980 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5980-Camera_UpdateTransform_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A5980-Camera_UpdateTransform_6.hpp"
int Camera_UpdateTransform_6(int context, int arg2, int arg3, char* arg4, char* arg5)

{
    char* source_val = arg5;
    short data_val = 0x61dc;

    if (g_9361e4)
    {
        int result = 0xff;
        unsigned int context_1 = 0;
        char* dest_2 = arg4;
        bool c_5;

        while (true)
        {
            result = result;

            if (!result)
                return result;

            result = *source_val;
            source_val = &source_val[1];
            context_1 = *dest_2;
            dest_2 = &dest_2[1];

            if (result != context_1)
            {
                int result_1 = result;
                unsigned int item_18_1 = context_1;
                unsigned int data_1;
                int entity_val;
                data_1 = Camera_SetFieldOfView_19(arg3);
                context_1 = data_1;
                result = Camera_SetFieldOfView_19(entity_val);
                c_5 = context_1 < result;

                if (context_1 != result)
                    break;
            }
        }

        return result - result + 1;
    }

    data_val = 0xff;
    char* dest_1 = arg4;

    while (true)
    {
        data_val = data_val;

        if (!data_val)
            break;

        data_val = *source_val;
        source_val = &source_val[1];
        *(&data_val + 1) = *dest_1;
        dest_1 = &dest_1[1];

        if (*(&data_val + 1) != data_val)
        {
            data_val -= 0x41;
            arg3 = arg3 - arg3;
            arg3 &= 0x20;
            data_val += arg3;
            data_val += 0x41;
            char f0_1 = data_val;
            data_val = *(&data_val + 1);
            *(&data_val + 1) = f0_1;
            data_val -= 0x41;
            arg3 = arg3 - arg3;
            arg3 &= 0x20;
            data_val += arg3;
            data_val += 0x41;
            bool c_3 = data_val < *(&data_val + 1);

            if (data_val != *(&data_val + 1))
            {
                char f3_1 = data_val;
                char f4_1 = data_val;
                data_val = f3_1 - f4_1;
                data_val = data_val + 1;
                break;
            }
        }
    }

    return data_val;
}
