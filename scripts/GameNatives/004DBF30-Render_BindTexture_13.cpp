// IGI native provenance: Render_BindTexture_13 - 0x004DBF30 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DBF30-Render_BindTexture_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004DBF30-Render_BindTexture_13.hpp"
void Render_BindTexture_13(int* context, void** arg2)

{
    void** i = arg2;

    while (i)
    {
        int data_1 = MagicObj_GetTypeId();
        short item_c_1 = data_1;
        data_1 = i[7];

        if (TaskType_IsDerivedFrom(data_1, item_c_1))
        {
            int* data_2 = i[0x1b];

            if (data_2 && *(*(*(*data_2 + 0xc) + 0xc) + 0x20) == 3)
                *context += data_2[0xf];

            int* entity_3 = i[2];
            int data_3 = *entity_3;
            int data_4 = -(data_3);
            Render_BindTexture_13(context, (data_4 - data_4) & entity_3);
        }

        i = *i;

        if (!i)
            break;

        if (!*i)
            break;
    }
}
