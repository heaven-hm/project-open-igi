// IGI native provenance: Camera_ApplyLineFilter_8 - 0x004A6181 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A6181-Camera_ApplyLineFilter_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyLineFilter_8.h"
int* Camera_ApplyLineFilter_8(char* context, int* arg2, char* arg3, int* arg4, int* arg5)

{
    int* entity_val = arg5;
    *entity_val = 0;
    char* source_val = arg3;
    int* dest_val = arg2;
    *arg4 = 1;
    char* data_1 = context;

    if (dest_val)
    {
        *dest_val = source_val;
        dest_val = &dest_val[1];
        arg2 = dest_val;
    }

    if (*data_1 != 0x22)
    {
        while (true)
        {
            *entity_val += 1;

            if (source_val)
            {
                *source_val = *data_1;
                source_val = &source_val[1];
            }

            char temp_5 = *data_1;
            data_1 = &data_1[1];

            if (*(temp_5 + 0xc32321) & 4)
            {
                *entity_val += 1;

                if (source_val)
                {
                    *source_val = *data_1;
                    source_val = &source_val[1];
                }

                data_1 = &data_1[1];
            }

            if (temp_5 == 0x20)
            {
            label_4a6228:

                if (temp_5)
                {
                    if (source_val)
                        source_val[0xffffffff] = 0;

                    break;
                }
            }
            else if (temp_5)
            {
                if (temp_5 == 9)
                    goto label_4a6228;

                continue;
            }

            data_1 -= 1;
            break;
        }
    }
    else
    {
        while (true)
        {
            char temp_1 = data_1[1];
            data_1 = &data_1[1];

            if (temp_1 == 0x22)
                break;

            if (!temp_1)
                break;

            if (*(temp_1 + 0xc32321) & 4)
            {
                *entity_val += 1;

                if (source_val)
                {
                    *source_val = *data_1;
                    source_val = &source_val[1];
                    data_1 = &data_1[1];
                }
            }

            *entity_val += 1;

            if (source_val)
            {
                *source_val = *data_1;
                source_val = &source_val[1];
            }
        }

        *entity_val += 1;

        if (source_val)
        {
            *source_val = 0;
            source_val = &source_val[1];
        }

        if (*data_1 == 0x22)
            data_1 = &data_1[1];
    }

    arg5 = nullptr;

    while (*data_1)
    {
        while (true)
        {
            char temp_6 = *data_1;

            if (temp_6 != 0x20 && temp_6 != 9)
                break;

            data_1 = &data_1[1];
        }

        if (!*data_1)
            break;

        if (dest_val)
        {
            *dest_val = source_val;
            dest_val = &dest_val[1];
            arg2 = dest_val;
        }

        *arg4 += 1;

        while (true)
        {
            context = 1;
            unsigned int i_2 = 0;

            while (*data_1 == 0x5c)
            {
                data_1 = &data_1[1];
                i_2 += 1;
            }

            if (*data_1 == 0x22)
            {
                if (!(i_2 & 1))
                {
                    if (!arg5 || data_1[1] != 0x22)
                        context = nullptr;
                    else
                        data_1 = &data_1[1];

                    dest_val = arg2;
                    int* temp_9 = nullptr;
                    temp_9 = !arg5;
                    arg5 = temp_9;
                }

                i_2 u>>= 1;
            }

            if (i_2)
            {
                int i_1 = i_2;
                int i;

                do
                {
                    if (source_val)
                    {
                        *source_val = 0x5c;
                        source_val = &source_val[1];
                    }

                    *entity_val += 1;
                    i = i_1;
                    i_1 -= 1;
                } while (i != 1);
            }

            char temp_11 = *data_1;

            if (!temp_11)
                break;

            if (!arg5)
            {
                if (temp_11 == 0x20)
                    break;

                if (temp_11 == 9)
                    break;
            }

            if (context)
            {
                if (source_val)
                {
                    if (*(temp_11 + 0xc32321) & 4)
                    {
                        *source_val = temp_11;
                        source_val = &source_val[1];
                        data_1 = &data_1[1];
                        *entity_val += 1;
                    }

                    *source_val = *data_1;
                    source_val = &source_val[1];
                }
                else if (*(temp_11 + 0xc32321) & 4)
                {
                    data_1 = &data_1[1];
                    *entity_val += 1;
                }

                *entity_val += 1;
            }

            data_1 = &data_1[1];
        }

        if (source_val)
        {
            *source_val = 0;
            source_val = &source_val[1];
        }

        *entity_val += 1;
    }

    if (dest_val)
        *dest_val = 0;

    *arg4 += 1;
    return arg4;
}
