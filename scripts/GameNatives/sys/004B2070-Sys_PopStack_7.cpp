// IGI native provenance: Sys_PopStack_7 - 0x004B2070 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B2070-Sys_PopStack_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B2070-Sys_PopStack_7.hpp"
int Sys_PopStack_7(void* context, int* arg2, int arg3, int arg4)

{
    int entity_val;
    int item_4 = entity_val;
    int result = 0;
    int result_2 = 0;
    int* dest_val = Sys_ResolveSymbol_19(context);

    if (dest_val)
    {
        int* flagP1 = arg2;

        while (result < arg3)
        {
            if (Sys_OpenFileStream_20(dest_val) == 0x48584554)
            {
                int* data_2 = Sys_CheckHeap_19(dest_val, context);
                dest_val = Matrix_InvalidateSlot(dest_val);

                if (!data_2 || !dest_val)
                    return 0;

                if (Sys_OpenFileStream_20(dest_val) != 0x59444f42)
                    return 0;

                if (!Sys_CheckHeap_19(dest_val, context))
                    return 0;

                void* data_6 = Sys_CheckHeap_19(dest_val, context);
                int entity_1 = 0;
                entity_1 = data_2[3];
                int data_8 = 0;
                data_8 = data_2[4];
                int data_9 = 0;
                data_9 = *(data_2 + 0x16);
                int temp_2 = 0;
                temp_2 = *(data_2 + 0x12);
                int entity_2 = 0;
                entity_2 = *(data_2 + 0xe);
                int data_11;
                int temp_3;
                temp_3 = HIGHD(data_9 * temp_2);
                data_11 = LOWD(data_9 * temp_2);
                Texture_UploadMipmapPacket(flagP1, arg4, (data_11 + (temp_3 & 7)) >> 3, entity_2, data_8, *data_2, entity_1,
                    data_2[1], data_6);
                int result_1 = result_2 + 1;
                flagP1 = &flagP1[9];
                result_2 = result_1;
                result = result_1;
            }

            dest_val = Matrix_InvalidateSlot(dest_val);

            if (!dest_val)
                break;
        }
    }

    return result;
}
