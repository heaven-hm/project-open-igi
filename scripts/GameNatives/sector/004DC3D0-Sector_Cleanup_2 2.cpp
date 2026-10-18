// IGI native provenance: Sector_Cleanup_2 - 0x004DC3D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DC3D0-Sector_Cleanup_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sector_Cleanup_2.h"
int* Sector_Cleanup_2(int* context, int* arg2)

{
    int item_c = 0;
    int* result_1 = nullptr;
    int item_4 = *arg2;
    int* i_2 = context;
    unsigned int data_2 = DynObj_FindSymbol_16();
    int data_3 = 0;
    data_3 = i_2[7];
    (*(((data_2 * 0x180 + data_3) << 2) + &g_a96ae0))(i_2, &item_4);

    if (i_2[0x52])
    {
        int* i = i_2;

        if (i_2)
        {
            do
            {
                void* data_6 = i[0x1b];

                if (data_6)
                {
                    result_1 += *(data_6 + 0x3c);
                    int* entity_1 = *(i[0x3a] + 8);
                    int data_8 = *entity_1;
                    int data_9 = -(data_8);
                    Render_BindTexture_13(&result_1, (data_9 - data_9) & entity_1);
                }

                if (i != i_2)
                {
                    i = *i;

                    if (!i)
                        break;

                    if (!*i)
                        break;
                }
                else
                {
                    void* source_1 = i[0x52];

                    if (!source_1)
                        break;

                    i = *(source_1 + 8);

                    if (!*i)
                        break;
                }
            } while (i);
        }
    }

    int* entity_2 = i_2[2];
    int data_12 = *entity_2;
    int data_13 = -(data_12);
    Render_ClipMesh_13(&result_1, (data_13 - data_13) & entity_2);
    int* result = result_1;

    if (result)
    {
        int** data_17 = Mem_Alloc(result * 0x24 + 0x18, 4);
        *data_17 = result_1;
        data_17[1] = i_2[0x1b];

        if (i_2[0x52])
        {
            int* i_1 = i_2;

            if (i_2)
            {
                do
                {
                    void* temp_5 = i_1[0x1b];

                    if (temp_5)
                    {
                        i_1[0x37] = &data_17[item_c * 9 + 6];
                        int j = 0;

                        if (*(temp_5 + 0x3c) > 0)
                        {
                            int entity_5 = item_c;
                            void* context_val = temp_5;

                            do
                            {
                                context_val += 4;
                                data_17[entity_5 * 9 + 6] = 0;
                                data_17[item_c * 9 + 8] = i_1;
                                data_17[item_c * 9 + 9] = &i_1[0x1c];
                                data_17[item_c * 9 + 0xa] = &i_1[8];
                                data_17[item_c * 9 + 0xb] = i_1[0x36];
                                data_17[item_c * 9 + 0xc] = j;
                                data_17[item_c * 9 + 0xd] = *(temp_5 + 0x3c);
                                data_17[item_c * 9 + 7] = *(context_val - 4);
                                data_17[item_c * 9 + 0xe] = 0;
                                entity_5 = item_c + 1;
                                j += 1;
                                item_c = entity_5;
                            } while (j < *(temp_5 + 0x3c));

                            i_2 = context;
                        }

                        int* entity_24 = *(i_1[0x3a] + 8);
                        int data_22 = *entity_24;
                        int data_23 = -(data_22);
                        int entity_25 = 0;
                        entity_25 = i_1 == i_2;
                        Render_DrawRigidMesh_14(&item_c, data_17, (data_23 - data_23) & entity_24, entity_25);
                    }

                    if (i_1 != i_2)
                    {
                        i_1 = *i_1;

                        if (!i_1)
                            break;

                        if (!*i_1)
                            break;
                    }
                    else
                    {
                        void* dest_3 = i_1[0x52];

                        if (!dest_3)
                            break;

                        i_1 = *(dest_3 + 8);

                        if (!*i_1)
                            break;
                    }
                } while (i_1);
            }
        }

        int* entity_26 = i_2[2];
        int data_26 = *entity_26;
        int data_27 = -(data_26);
        Render_DrawBoneMesh_14(&item_c, data_17, (data_27 - data_27) & entity_26);
        Render_SkinBones_14(i_2, &data_17[3], &data_17[2]);
        result = arg2;
        *result = data_17;
    }

    return result;
}
