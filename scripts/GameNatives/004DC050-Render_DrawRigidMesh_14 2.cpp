// IGI native provenance: Render_DrawRigidMesh_14 - 0x004DC050 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DC050-Render_DrawRigidMesh_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_DrawRigidMesh_14.h"
void Render_DrawRigidMesh_14(int* context, int arg2, int* arg3, int arg4)

{
    int* i = arg3;

    while (i)
    {
        int* flagP1 = i[0x1b];
        int data_1 = MagicObj_GetTypeId();
        short item_14_1 = data_1;
        data_1 = i[7];

        if (TaskType_IsDerivedFrom(data_1, item_14_1))
        {
            int data_val = i[0x1b];

            if (data_val && *(*(*(*data_val + 0xc) + 0xc) + 0x20) == 3)
            {
                int j = 0;
                i[0x37] = arg2 + *context * 0x24 + 0x18;

                if (flagP1[0xf] > 0)
                {
                    arg3 = flagP1;

                    do
                    {
                        *(arg2 + *context * 0x24 + 0x18) = 0;
                        *(arg2 + *context * 0x24 + 0x20) = i;
                        *(arg2 + (*context + 1) * 0x24) = &i[0x1c];
                        *(arg2 + *context * 0x24 + 0x28) = &i[8];
                        *(arg2 + *context * 0x24 + 0x2c) = i[0x36];
                        *(arg2 + *context * 0x24 + 0x30) = j;
                        *(arg2 + *context * 0x24 + 0x34) = flagP1[0xf];
                        *(arg2 + *context * 0x24 + 0x1c) = *arg3;
                        *(arg2 + *context * 0x24 + 0x38) = 0;
                        j += 1;
                        *context += 1;
                        arg3 = &arg3[1];
                    } while (j < flagP1[0xf]);
                }

                int* entity_3 = i[2];
                int data_26 = *entity_3;
                int data_27 = -(data_26);
                Render_DrawRigidMesh_14(context, arg2, (data_27 - data_27) & entity_3, arg4);
            }
        }

        i = *i;

        if (!i)
            break;

        if (!*i)
            break;
    }
}
