// IGI native provenance: SplineObjSegment_Setup - 0x004E4290 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E4290-SplineObjSegment_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SplineObjSegment_Setup.h"
int SplineObjSegment_Setup(void* context)

{
    void* item_54 = context;
    void item_4c;
    void* item_50 = Render_SkinBones_13(&item_4c);
    void* item_48;
    void item_44;
    Render_ComputeNormals_12(&item_48, &item_44);
    (*((DynObj_LinkDynamicCube_15() << 2) + &g_a94e84))(&item_54);
    void item_40;
    memcpy(&item_40, context + 0x20, 0x18);
    int* i = *(context + 8);
    int item_28 = 0x3f800000;
    int item_24 = 0;
    int item_20 = 0;
    int item_1c = 0;
    int item_18 = 0x3f800000;
    int item_14 = 0;
    int item_10 = 0;
    int item_c = 0;
    int item_8 = 0x3f800000;
    int item_4 = 0;
    int result = *i;

    if (result && i)
    {
        int entity_1 = g_afa7e0;

        do
        {
            int* temp_1 = *i;
            void* data_5;

            if (!temp_1)
                data_5 = nullptr;
            else
            {
                int data_2 = *temp_1;
                int data_3 = -(data_2);
                data_5 = (data_3 - data_3) & temp_1;
            }

            *((entity_1 << 2) + &g_afa6e0) = data_5;
            g_afa7e0 = entity_1 + 1;
            unsigned int data_7 = DynObj_FindSymbol_6();
            int data_8 = 0;
            data_8 = i[7];
            result = *(((data_7 * 0x180 + data_8) << 2) + &g_a96ae0);

            if (result)
                result = result(i, &item_40);

            entity_1 = g_afa7e0 - 1;
            g_afa7e0 = entity_1;
            i = *((entity_1 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
