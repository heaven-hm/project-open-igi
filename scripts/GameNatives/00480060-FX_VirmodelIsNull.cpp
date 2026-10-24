// IGI native provenance: FX_VirmodelIsNull - 0x00480060 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00480060-FX_VirmodelIsNull.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00480060-FX_VirmodelIsNull.hpp"
struct status_msg* FX_VirmodelIsNull(double* context, char* arg2, float arg3, int arg4, int arg5, int arg6 , int arg7, int arg8, int arg9)

{
    int kind;
    kind = g_5c1168;
    struct status_msg* result = QTask_HandleInput(Script_FormatString(), kind, 0);
    void* data_1 = DynObj_Missing(arg2);

    if (data_1)
    {
        DynObj_CreateMagicObject_5(result, data_1);
        long double fpu7 = result->head[0x27];
        long double fpu6 = result->head[0x28];
        fpu6 - fpu7;
        long double fpu_val_7_1;

        if (!(*(&((fpu6 < fpu7 ? 1 : 0) << 8 | (FCMP_UO(fpu6, fpu7) ? 1 : 0) << 0xa
                | (fpu6 == fpu7 ? 1 : 0) << 0xe) + 1) & 1))
            fpu_val_7_1 = result->head[0x28];
        else
            fpu_val_7_1 = result->head[0x27];

        long double fpu_val_6_1 = result->head[0x26];
        fpu_val_6_1 - fpu_val_7_1;
        long double fpu_val_7_3;

        if (*(&((fpu_val_6_1 < fpu_val_7_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_1, fpu_val_7_1) ? 1 : 0) << 0xa
            | (fpu_val_6_1 == fpu_val_7_1 ? 1 : 0) << 0xe | 0x3800) + 1) & 0x41)
        {
            long double fpu_val_7_4 = result->head[0x27];
            long double fpu_val_6_2 = result->head[0x28];
            fpu_val_6_2 - fpu_val_7_4;

            if (!(*(&((fpu_val_6_2 < fpu_val_7_4 ? 1 : 0) << 8
                    | (FCMP_UO(fpu_val_6_2, fpu_val_7_4) ? 1 : 0) << 0xa
                    | (fpu_val_6_2 == fpu_val_7_4 ? 1 : 0) << 0xe) + 1) & 1))
                fpu_val_7_3 = result->head[0x28];
            else
                fpu_val_7_3 = result->head[0x27];
        }
        else
            fpu_val_7_3 = result->head[0x26];

        float item_c_2 = Render_ComputeNormals(result->head[0x1b]) * fpu_val_7_3;
        WorldObject_CommitTransform(Level_StartMission_4(), result, context, item_c_2);
        result->sprite[0x54] = arg7;
        result->sprite[0x55] = *(&arg7 + 1);
        result->sprite[0x56] = *(&arg7 + 2);
        result->sprite[0x57] = *(&arg7 + 3);
        result->head[0x26] = 0x3a83126f;
        result->head[0x27] = 0x3a83126f;
        result->head[0x28] = 0x3a83126f;
        result->sprite[0x40] = arg4;
        result->sprite[0x41] = *(&arg4 + 1);
        result->sprite[0x42] = *(&arg4 + 2);
        result->sprite[0x43] = *(&arg4 + 3);
        result->sprite[0x44] = arg4;
        result->sprite[0x45] = *(&arg4 + 1);
        result->sprite[0x46] = *(&arg4 + 2);
        result->sprite[0x47] = *(&arg4 + 3);
        result->sprite[0x48] = arg6;
        result->sprite[0x49] = *(&arg6 + 1);
        result->sprite[0x4a] = *(&arg6 + 2);
        result->sprite[0x4b] = *(&arg6 + 3);
        result->sprite[0x4c] = arg5;
        result->sprite[0x4d] = *(&arg5 + 1);
        result->sprite[0x4e] = *(&arg5 + 2);
        result->sprite[0x4f] = *(&arg5 + 3);
        result->sprite[0x50] = arg5;
        result->sprite[0x51] = *(&arg5 + 1);
        result->sprite[0x52] = *(&arg5 + 2);
        result->sprite[0x53] = *(&arg5 + 3);
        void* item_1c = data_1;
        result->sprite[0x58] = arg3 * 0.00048828125;
        char* item_20 = &result->sprite[0xc];
        result->sprite[0x5c] = arg8;
        result->sprite[0x5d] = *(&arg8 + 1);
        result->sprite[0x5e] = *(&arg8 + 2);
        result->sprite[0x5f] = *(&arg8 + 3);
        result->sprite[0x60] = arg9;
        result->sprite[0x61] = *(&arg9 + 1);
        result->sprite[0x62] = *(&arg9 + 2);
        result->sprite[0x63] = *(&arg9 + 3);
        return result;
    }

    Error_Show("VirModel is null");

    while (true)
        /* nop */
}
