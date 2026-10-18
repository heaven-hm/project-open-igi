// IGI native provenance: Light_CalculateAttenuation_11 - 0x0052D9F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052D9F0-Light_CalculateAttenuation_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Light_CalculateAttenuation_11.hpp"
unsigned int Light_CalculateAttenuation_11(int context)

{
    int source_val = context;
    int item_18 = 0;
    int data_val = *(source_val + 0x4ab0);
    int i = 1;
    void* entity_val = source_val + 0x48c0;
    long long item_8 = data_val * 0.703000009f;
    int data_1 = item_8;
    int temp_3;
    int dest_1;

    do
    {
        context = i;
        int j_1 = 5;
        int j;

        do
        {
            dest_1 = *(source_val + 0x4aa8);
            int item_c_1 = 0;
            long double fpu_val_5_2 = context * 0.5f;
            *(&item_8 + 4) = 0;
            entity_val += 0x10;
            temp_3 = 0;
            item_8 = COMBINE(temp_3, dest_1) % data_val;
            j = j_1;
            j_1 -= 1;
            context += i;
            long double fp0_1;
            bool c1_1;
            bool c2_1;
            fp0_1 = __fsin(item_8 / data_val * 6.2831853071795862 + fpu_val_5_2);
            *(entity_val - 0x18) = fp0_1 * *(source_val + 0x4a88);
            long double fp0_2;
            bool c1_2;
            bool c2_2;
            fp0_2 =
                __fsin((COMBINE(0, dest_1 + 0xbd) % data_1) / data_1 * 6.2831853071795862 + fpu_val_5_2);
            *(entity_val - 0x10) = fp0_2 * *(source_val + 0x4a88);
        } while (j != 1);
        i += 1;
    } while (i <= 5);

    return COMBINE(temp_3, dest_1) / data_val;
}
