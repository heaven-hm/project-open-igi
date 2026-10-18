// IGI native provenance: Sys_SIsNotAValidTextureF - 0x004B24C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B24C0-Sys_SIsNotAValidTextureF.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B24C0-Sys_SIsNotAValidTextureF.hpp"
int Sys_SIsNotAValidTextureF(int* context, int arg2, int* arg3)

{
    int* item_10 = arg3;
    int data_val;
    void* entity_val;
    int temp_val;
    int* data_1 = Resource_Load(data_val, temp_val, entity_val, nullptr);
    char data_2 = *data_1;

    if (data_2 == 0x4c)
    {
        int* item_2c;
        int item_28;
        int item_24;
        int item_20;
        int item_1c;
        int item_18;
        int item_14;
        int item_10_1;
        void* item_c;

        if (*(data_1 + 1) == 0x4f && *(data_1 + 2) == 0x4f && *(data_1 + 3) == 0x50 && data_1[1] == 2)
        {
            Texture_EnableUpload();
            int entity_3 = 0;
            entity_3 = data_1[2];
            int temp_3 = 0;
            temp_3 = *(data_1 + 0x16);
            item_c = &data_1[6];
            item_10_1 = 0;
            item_14 = entity_3;
            int data_4 = 0;
            item_18 = temp_3;
            data_4 = data_1[5];
            int entity_4 = 0;
            entity_4 = *(data_1 + 0x12);
            int temp_4 = 0;
            temp_4 = data_1[4];
            item_1c = data_4;
            item_20 = entity_4;
            item_24 = temp_4;
            item_28 = arg2;
            item_2c = context;
        label_4b252e:
            Texture_UploadMipmapPacket(item_2c, item_28, item_24, item_20, item_1c, item_18, item_14, item_10_1, item_c);
            int result = Resource_Unload(arg3);
            Texture_DisableUpload();
            return result;
        }

        if (data_2 == 0x4c && *(data_1 + 1) == 0x4f && *(data_1 + 2) == 0x4f && *(data_1 + 3) == 0x50
            && data_1[1] == 0xb)
        {
            Texture_EnableUpload();
            item_c = &data_1[8];
            int entity_6 = 0;
            item_10_1 = data_1[3];
            entity_6 = data_1[5];
            int data_7 = 0;
            data_7 = data_1[6];
            item_14 = entity_6;
            item_18 = data_1[2];
            item_1c = data_7;
            int temp_7 = 0;
            int data_8 = 0;
            temp_7 = *(data_1 + 0x1e);
            data_8 = *(data_1 + 0x1a);
            int entity_7 = 0;
            entity_7 = *(data_1 + 0x16);
            item_20 = entity_7;
            item_24 = temp_7 * data_8;
            item_28 = arg2;
            item_2c = context;
            goto label_4b252e;
        }
    }

    int* item_c_1 = arg3;
    Error_Show("'%s' is not a valid Texture file.");

    while (true)
        /* nop */
}
