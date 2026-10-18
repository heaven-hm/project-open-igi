// IGI native provenance: SoundSys_SetMasterVolume_8 - 0x004E6DB0 - CFG score 96.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E6DB0-SoundSys_SetMasterVolume_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_SetMasterVolume_8.h"
void SoundSys_SetMasterVolume_8(int context, int arg2, int arg3, int arg4, float* arg5)

{
    int* flagP = g_a5e5f4;
    int context_val = 0;

    if (!flagP)
    {
    label_4e6ddd:
        int* flagP1 = g_a5e5ec;

        if (flagP1)
        {
            float* source_1 = arg5;
            float* data_1 = &flagP1[8];
            arg5 = data_1;

            if (!source_1)
            {
                Display_SetGammaRamp_12(data_1);
                data_1 = arg5;
            }
            else
                memcpy(data_1, source_1, 0x58);

            if (g_a5e60b)
            {
            label_4e6f65:
                flagP1[7] = context_val;
                flagP1[3] = arg2;
                flagP1[4] = arg3;
                flagP1[2] = arg4;
                flagP1[5] = context;
                flagP1[6] = 0;
                SoundSys_AttachLipsync_4(flagP1, &g_a5e5ec);
                SoundSys_LoadSoundDef_4(flagP1, &g_a5e5f4);
            }
            else
            {
                context_val = Display_SwapBuffers_11(arg2, arg3, arg4, data_1);

                if (context_val)
                {
                    long double fpu_val_7_3 = *arg5;
                    long double f0_1 = 0.0f;
                    fpu_val_7_3 - f0_1;

                    if (!(*(&((fpu_val_7_3 < f0_1 ? 1 : 0) << 8
                            | (FCMP_UO(fpu_val_7_3, f0_1) ? 1 : 0) << 0xa
                            | (fpu_val_7_3 == f0_1 ? 1 : 0) << 0xe) + 1) & 0x40))
                        goto label_4e6f65;

                    Display_SetDisplayMode_12(context_val);
                    Display_CloseDevice_11(context_val);
                    return;
                }

                if (g_a5e60b)
                    goto label_4e6f65;
            }
        }
    }
    else
    {
        while (flagP[4] != arg3 || flagP[5] != context)
        {
            flagP = flagP[1];

            if (!flagP)
                goto label_4e6ddd;
        }

        if (!flagP)
            goto label_4e6ddd;

        flagP[6] = 0;

        if (!arg5)
            Display_SetGammaRamp_12(&flagP[8]);
        else
            memcpy(&flagP[8], arg5, 0x58);

        int data_2 = flagP[7];

        if (!data_2)
        {
            if (g_a5e60b)
                goto label_4e6ea8;

            int data_5 = Display_SwapBuffers_11(arg2, arg3, arg4, &flagP[8]);

            if (!data_5)
            {
            label_4e6eda:
                SoundSys_AttachLipsync_4(flagP, &g_a5e5f4);
                SoundSys_LoadSoundDef_4(flagP, &g_a5e5ec);
                return;
            }

            long double fpu_val_7_1 = flagP[8];
            long double f2_1 = 0.0f;
            fpu_val_7_1 - f2_1;

            if (!(*(&((fpu_val_7_1 < f2_1 ? 1 : 0) << 8
                | (FCMP_UO(fpu_val_7_1, f2_1) ? 1 : 0) << 0xa
                | (fpu_val_7_1 == f2_1 ? 1 : 0) << 0xe) + 1) & 0x40))
            {
                flagP[7] = data_5;
                goto label_4e6ea8;
            }

            Display_SetDisplayMode_12(data_5);
            Display_CloseDevice_11(data_5);
            SoundSys_AttachLipsync_4(flagP, &g_a5e5f4);
            SoundSys_LoadSoundDef_4(flagP, &g_a5e5ec);
            return;
        }

        Display_SyncVSync_12(data_2, &flagP[8]);
    label_4e6ea8:
        long double fpu_val_7_2 = flagP[8];
        long double f1_1 = 0.0f;
        fpu_val_7_2 - f1_1;

        if (*(&((fpu_val_7_2 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_2, f1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_2 == f1_1 ? 1 : 0) << 0xe) + 1) & 0x40)
        {
            void* data_9 = flagP[7];

            if (data_9)
            {
                Display_SetDisplayMode_12(data_9);
                Display_CloseDevice_11(flagP[7]);
            }

            goto label_4e6eda;
        }
    }
}
