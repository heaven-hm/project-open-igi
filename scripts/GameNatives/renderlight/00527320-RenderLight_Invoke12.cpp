// IGI native provenance: RenderLight_Invoke12 - 0x00527320 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00527320-RenderLight_Invoke12.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00527320-RenderLight_Invoke12.hpp"
int RenderLight_Invoke12(void* arg1)

{
    if (!data_a83d54)
        data_a83d54 = arg1;

    long double x87_r7_1 = 6.28318548f - *(arg1 + 0x24);
    int __saved_index_2;
    int __saved_index = __saved_index_2;
    *(arg1 + 0x28) = 0x3f800000;
    *(arg1 + 0x2c) = 0;
    *(arg1 + 0x30) = 0;
    int __saved_index_3 = __saved_index_2;
    float var_28;
    Mat33_FromEulerAngles(&var_28, 0f, 6.28318548f - *(arg1 + 0x20), x87_r7_1);
    float var_1c;
    float var_18;
    float var_14;
    float var_30 = var_14 * *(arg1 + 0x30) + var_18 * *(arg1 + 0x2c) + var_1c * *(arg1 + 0x28);
    float var_c;
    float var_8;
    long double x87_r7_14 = var_8 * *(arg1 + 0x30) + var_c * *(arg1 + 0x2c);
    float var_10;
    long double x87_r6_11 = var_10 * *(arg1 + 0x28);
    float var_24;
    float var_20;
    *(arg1 + 0x28) = var_20 * *(arg1 + 0x30) + var_24 * *(arg1 + 0x2c) + var_28 * *(arg1 + 0x28);
    *(arg1 + 0x2c) = var_30;
    long double x87_r7_16 = *(arg1 + 0x4c);
    *(arg1 + 0x50) = x87_r7_16;
    *(arg1 + 0x30) = x87_r7_14 + x87_r6_11;
    data_a83d44 = x87_r7_16;
    void* eax_1 = *(arg1 + 0x14);

    if (eax_1 && *(eax_1 + 0x1c) == g_a83d34)
    {
        unsigned int eax_3 = Script_ParseEnumReal32_8();
        int eax_4 = 0;
        eax_4 = *(eax_1 + 0x1c);
        (*(((eax_3 * 0x180 + eax_4) << 2) + &g_a96ae0))(eax_1, arg1);
    }

    int eax_6 = Light_CalculateAttenuation();
    int eax_7 = Script_ParseEnumReal32_8();
    QTask_CreateTask_3(Script_ParseEnumReal32_7(), eax_7, eax_6, 0);
    int result = data_a83d54;

    if (result == arg1)
        data_a83d54 = 0;

    return result;
}
