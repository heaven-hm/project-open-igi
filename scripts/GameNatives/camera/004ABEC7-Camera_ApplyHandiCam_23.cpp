// IGI native provenance: Camera_ApplyHandiCam_23 - 0x004ABEC7 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ABEC7-Camera_ApplyHandiCam_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004ABEC7-Camera_ApplyHandiCam_23.hpp"
int Camera_ApplyHandiCam_23(enum WIN32_ERROR context, char* arg2, unsigned int arg3)

{
    char data_4;
    int* context_1;
    int source_3;

    if (context < g_c32540)
    {
        context_1 = &(&g_c32440)[context >> 5];
        source_3 = (context & ERROR_GEN_FAILURE) << 3;
        data_4 = *(*context_1 + source_3 + 4);
    }

    if (context >= g_c32540 || !(data_4 & 1))
    {
        g_936068 = 0;
        g_936064 = 9;
    }
    else
    {
        unsigned int numberOfBytesWritten_1 = 0;
        int item_14_1 = 0;

        if (!arg3)
            return 0;

        if (data_4 & 0x20)
            Camera_ResetMatrix_9(context, 0, FILE_END);

        int* data_6 = *context_1 + source_3;
        unsigned int numberOfBytesWritten;

        if (!(data_6[1] & 0x80))
        {
            if (!WriteFile(*data_6, arg2, arg3, &numberOfBytesWritten, nullptr))
                context = GetLastError();
            else
            {
                context = NO_ERROR;
                numberOfBytesWritten_1 = numberOfBytesWritten;
            }

        label_4abfc3:

            if (numberOfBytesWritten_1)
                return numberOfBytesWritten_1 - item_14_1;

            if (!context)
                goto label_4ac031;

            if (context != ERROR_ACCESS_DENIED)
                Camera_UpdateViewCone_22(context);
            else
            {
                g_936064 = 9;
                g_936068 = 5;
            }
        }
        else
        {
            char* item_8_1 = arg2;
            context = NO_ERROR;

            if (arg3 > 0)
            {
                do
                {
                    unsigned char buffer[0x404];
                    unsigned char (* data_8)[0x404] = &buffer;

                    while (item_8_1 - arg2 < arg3)
                    {
                        char* entity_3 = item_8_1;
                        item_8_1 = &item_8_1[1];
                        char entity_4 = *entity_3;

                        if (entity_4 == 0xa)
                        {
                            item_14_1 += 1;
                            *data_8 = 0xd;
                            data_8 = &(*data_8)[1];
                        }

                        *data_8 = entity_4;
                        data_8 = &(*data_8)[1];

                        if (data_8 - &buffer >= 0x400)
                            break;
                    }

                    unsigned int nNumberOfBytesToWrite = data_8 - &buffer;

                    if (!WriteFile((*context_1)[(context & 0x1f) * 2], &buffer, nNumberOfBytesToWrite,
                        &numberOfBytesWritten, nullptr))
                    {
                        context = GetLastError();
                        break;
                    }

                    unsigned int numberOfBytesWritten_2 = numberOfBytesWritten;
                    numberOfBytesWritten_1 += numberOfBytesWritten_2;

                    if (numberOfBytesWritten_2 < nNumberOfBytesToWrite)
                        break;
                } while (item_8_1 - arg2 < arg3);

                goto label_4abfc3;
            }

        label_4ac031:

            if (*(*context_1 + source_3 + 4) & 0x40 && *arg2 == 0x1a)
                return 0;

            g_936064 = 0x1c;
            g_936068 = 0;
        }
    }

    return 0xffffffff;
}
