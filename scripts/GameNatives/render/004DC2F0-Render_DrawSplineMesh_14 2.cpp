// IGI native provenance: Render_DrawSplineMesh_14 - 0x004DC2F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DC2F0-Render_DrawSplineMesh_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Render_DrawSplineMesh_14.h"
void Render_DrawSplineMesh_14(int* context)

{
    int* i = context;

    while (i)
    {
        int data_1 = MagicObj_GetTypeId();
        short item_8_1 = data_1;
        data_1 = i[7];

        if (TaskType_IsDerivedFrom(data_1, item_8_1))
        {
            i[0x37] = 0;
            int* entity_1 = i[2];
            int data_2 = *entity_1;
            int data_3 = -(data_2);
            Render_DrawSplineMesh_14((data_3 - data_3) & entity_1);
        }

        i = *i;

        if (!i)
            break;

        if (!*i)
            break;
    }
}
