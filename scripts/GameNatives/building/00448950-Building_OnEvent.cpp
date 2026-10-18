// IGI native provenance: Building_OnEvent - 0x00448950 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00448950-Building_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Building_OnEvent.hpp"
int Building_OnEvent(float context, void* arg2)

{
    long double fpu7 = *(context + 0x9c);
    long double fpu6 = *(context + 0xa0);
    fpu6 - fpu7;
    long double fpu_val_7_1;

    if (!(*(&((fpu6 < fpu7 ? 1 : 0) << 8 | (FCMP_UO(fpu6, fpu7) ? 1 : 0) << 0xa
            | (fpu6 == fpu7 ? 1 : 0) << 0xe) + 1) & 1))
        fpu_val_7_1 = *(context + 0xa0);
    else
        fpu_val_7_1 = *(context + 0x9c);

    long double fpu_val_6_1 = *(context + 0x98);
    fpu_val_6_1 - fpu_val_7_1;
    long double fpu_val_7_3;

    if (*(&((fpu_val_6_1 < fpu_val_7_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_1, fpu_val_7_1) ? 1 : 0) << 0xa
        | (fpu_val_6_1 == fpu_val_7_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
    {
        long double fpu_val_7_4 = *(context + 0x9c);
        long double fpu_val_6_2 = *(context + 0xa0);
        fpu_val_6_2 - fpu_val_7_4;

        if (!(*(&((fpu_val_6_2 < fpu_val_7_4 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_2, fpu_val_7_4) ? 1 : 0) << 0xa
                | (fpu_val_6_2 == fpu_val_7_4 ? 1 : 0) << 0xe) + 1) & 1))
            fpu_val_7_3 = *(context + 0xa0);
        else
            fpu_val_7_3 = *(context + 0x9c);
    }
    else
        fpu_val_7_3 = *(context + 0x98);

    long double fpu_val_7_6 = Render_ComputeNormals(*(context + 0x6c)) * fpu_val_7_3;
    DynObj_LinkDynamicCube_8(*(arg2 + 4), context, context + 0x1d8, fpu_val_7_6, 8);
    (*(((DynObj_UpdateDynTree_12() * 0x180 + Render_ComputeNormals_15()) << 2) + &g_a96ae0))(context, arg2);
    int* i = *(context + 8);
    int result = *i;

    if (result && i)
    {
        int entity_1 = g_afa7e0;

        do
        {
            int* temp_1 = *i;
            void* data_10;

            if (!temp_1)
                data_10 = nullptr;
            else
            {
                int data_7 = *temp_1;
                int data_8 = -(data_7);
                data_10 = (data_8 - data_8) & temp_1;
            }

            *((entity_1 << 2) + &g_afa6e0) = data_10;
            g_afa7e0 = entity_1 + 1;
            unsigned int data_12 = DynObj_UpdateDynTree_12();
            int entity_3 = 0;
            entity_3 = i[7];
            result = *(((data_12 * 0x180 + entity_3) << 2) + &g_a96ae0);

            if (result)
                result = result(i, arg2);

            entity_1 = g_afa7e0 - 1;
            g_afa7e0 = entity_1;
            i = *((entity_1 << 2) + &g_afa6e0);
        } while (i);
    }

    return result;
}
