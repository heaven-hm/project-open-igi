// IGI native provenance: Render_ClipMesh - 0x004D0A50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D0A50-Render_ClipMesh.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D0A50-Render_ClipMesh.hpp"
double* Render_ClipMesh(double* context, double* arg2, int arg3)

{
    double* result = arg2;
    int* entity_val = *(result + 0x6c);

    if (entity_val)
    {
        int* entity_4 = *(*(*(*entity_val + 0xc) + 0xc) + 0x88) + (arg3 << 4);
        long double fpu_val_7_2 = *entity_4 * result[0x13];
        float item_14_2 = entity_4[1] * *(result + 0x9c);
        long double fpu_val_6_4 = entity_4[2] * result[0x14];
        float item_10_2 = fpu_val_6_4;
        float item_c_1 =
            fpu_val_6_4 * result[0xf] + item_14_2 * *(result + 0x74) + fpu_val_7_2 * result[0xe];
        long double fpu_val_6_11 =
            item_10_2 * *(result + 0x84) + item_14_2 * result[0x10] + fpu_val_7_2 * *(result + 0x7c);
        long double fpu_val_5_11 = item_10_2 * result[0x12] + item_14_2 * *(result + 0x8c);
        long double fpu_val_5_13 = fpu_val_7_2 * result[0x11];
        result = context;
        *result = item_c_1;
        result[1] = fpu_val_6_11;
        result[2] = fpu_val_5_11 + fpu_val_5_13;
    }

    return result;
}
