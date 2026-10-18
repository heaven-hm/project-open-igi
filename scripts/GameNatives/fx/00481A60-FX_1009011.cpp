// IGI native provenance: FX_1009011 - 0x00481A60 - CFG score 94.4% (PASS; structural only); runtime behavior not verified.
// Evidence: 00481A60-FX_1009011.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00481A60-FX_1009011.hpp"
struct status_msg* FX_1009011(double* context)

{
    void* data_1 = DynObj_Missing(&context[0x30]);
    double item_18;
    memcpy(&item_18, &context[4], 0x18);
    double* item_30;

    if (data_1)
    {
        item_30 = context;
        DynObj_CreateMagicObject_5(item_30, data_1);
        int* i = context[1];
        *(context + 0xdc) = 0;

        if (*i && i)
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
                int data_6 = Render_TransformVertices_2();
                int temp_2 = 0;
                temp_2 = i[7];
                int data_7 = *(((data_6 * 0x180 + temp_2) << 2) + &g_a96ae0);

                if (data_7)
                    data_7(i);

                entity_1 = g_afa7e0 - 1;
                g_afa7e0 = entity_1;
                i = *((entity_1 << 2) + &g_afa6e0);
            } while (i);
        }
    }

    int item_2c_3 = 0;
    item_30 = context;
    WeaponType_UpdateAnim(&item_18, item_30, item_2c_3);
    long double fpu_val_7_1 = item_18 + context[4];
    unsigned int temp_3 = context[0x42];
    unsigned int data_8 = *(context + 0x20c);
    unsigned int item_2c_4 = 0;
    item_30 = 0x47c80000;
    item_18 = fpu_val_7_1;
    double item_10;
    double item_10_1 = item_10 + context[5];
    void* item_54 = context + 0x214;
    double item_8;
    double item_8_1 = item_8 + context[6];
    void* item_58 = context + 0x214;
    struct status_msg* result = FX_SmokeEmit(&item_18, 1, 0, context[0x40], context[0x3f] * 4096.0f,
        *(context + 0x1fc) * 4096.0f, 0x45c00000, data_8, temp_3, nullptr, "1009_01_1", context + 0x214,
        0x15, 2, item_30, item_2c_4);
    context[0x32] = 1;
    return result;
}
