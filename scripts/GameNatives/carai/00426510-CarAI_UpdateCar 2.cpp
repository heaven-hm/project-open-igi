// IGI native provenance: CarAI_UpdateCar - 0x00426510 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00426510-CarAI_UpdateCar.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "CarAI_UpdateCar.h"
int CarAI_UpdateCar(void* context, int arg2, int arg3, int arg4, float arg5)

{
    long double fpu7 = arg5;
    long double f0 = 0.0f;
    fpu7 - f0;
    int item_14;
    int* item_18 = &item_14;
    int item_1c = arg2;
    item_14 = arg4;
    int item_10 = arg3;
    long double fpu_val_7_1;

    fpu_val_7_1 = !(*(&((fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe) + 1) & 0x40) ? arg5 : -1.0f;

    float item_8 = fpu_val_7_1;
    void* source_val = *(context + 0x4c);
    unsigned int data_3 = Script_ParseEnumReal32_15();
    int data_4 = 0;
    data_4 = *(source_val + 0x1c);
    return (*(((data_3 * 0x180 + data_4) << 2) + &g_a96ae0))(source_val, &item_1c);
}
