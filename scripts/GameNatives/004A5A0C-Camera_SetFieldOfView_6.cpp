// IGI native provenance: Camera_SetFieldOfView_6 - 0x004A5A0C - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004A5A0C-Camera_SetFieldOfView_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A5A0C-Camera_SetFieldOfView_6.hpp"
void* Camera_SetFieldOfView_6(char* context, char* arg2)

{
    int context_val;
    int item_28 = context_val;
    char* source_val = arg2;
    char item_24[0x20];
    memset(&item_24, 0, 0x20);
    char i;

    do
    {
        i = *source_val;
        unsigned int i_1 = i;
        int saved_frame;
        *(&saved_frame + (i_1 >> 3) - 0x20) |= 1 << (i_1 & 7);
        source_val = &source_val[1];
    } while (i);
    char* temp_1 = context;

    if (!temp_1)
        temp_1 = g_936060;

    char data_4;

    while (true)
    {
        data_4 = *temp_1;
        unsigned int source_1 = data_4;

        if (!(item_24[source_1 >> 3] & 1 << (source_1 & 7)))
            break;

        if (!data_4)
            break;

        temp_1 = &temp_1[1];
    }

    char* context_3 = temp_1;

    while (true)
    {
        data_4 = *temp_1;

        if (!data_4)
            goto label_4a5a96;

        unsigned int source_3 = data_4;

        if (item_24[source_3 >> 3] & 1 << (source_3 & 7))
        {
            *temp_1 = 0;
            temp_1 = &temp_1[1];
        label_4a5a96:
            int data_7 = context_3 - temp_1;
            int data_8 = -(data_7);
            g_936060 = temp_1;
            return (data_8 - data_8) & context_3;
        }

        temp_1 = &temp_1[1];
    }
}
