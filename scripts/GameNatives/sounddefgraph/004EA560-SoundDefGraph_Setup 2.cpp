// IGI native provenance: SoundDefGraph_Setup - 0x004EA560 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EA560-SoundDefGraph_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundDefGraph_Setup.h"
int* SoundDefGraph_Setup(void* context, int* arg2)

{
    if (*(context + 0xd4) && arg2[1])
        return arg2;

    if (*(context + 0x2c) & 2)
    {
        double* data_3 = SoundSysMicrophone_Action(*arg2);
        double item_8_1 = *(context + 0xd0) * 4096.0f;
        int* data_1;
        long double fp0_1;
        fp0_1 = Sys_PushStack_11(data_3, context + 0x88);
        long double fpu_val_6_2 = item_8_1 * item_8_1;
        fpu_val_6_2 - fp0_1;
        data_1 = (fpu_val_6_2 < fp0_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_6_2, fp0_1) ? 1 : 0) << 0xa
            | (fpu_val_6_2 == fp0_1 ? 1 : 0) << 0xe;

        if (*(&data_1 + 1) & 1)
            return data_1;
    }

    int data_4 = *(context + 0x184);

    if (data_4 != *(context + 0x24))
    {
        int temp_1 = *(context + 0x180);
        *(context + 0x24) = data_4;
        *(context + 0x20) = temp_1;
        *(context + 0x28) = 0;
    }

    *(context + 0x2c) |= 0x10;
    return SoundSys_Update3DPosition_8(context + 0x20);
}
