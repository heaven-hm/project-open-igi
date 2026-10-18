// IGI native provenance: SoundSysMicrophone_OnTick - 0x004EBDF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EBDF0-SoundSysMicrophone_OnTick.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSysMicrophone_OnTick.h"
int SoundSysMicrophone_OnTick(int* context)

{
    int result;
    result = context[2];

    if (!result)
    {
        char* source_1 = &g_547f48;

        if (!FX_FireBallUpdate_27())
            source_1 = &g_547f44;

        int temp_1 = *context;
        int i = 0xffffffff;
        int dest_1 = temp_1;
        result = 0;

        while (i)
        {
            bool flag_1_1 = 0 != *dest_1;
            dest_1 += 1;
            i -= 1;

            if (!flag_1_1)
                break;
        }

        char* dest_2 = source_1;
        int i_1 = 0xffffffff;

        while (i_1)
        {
            bool flag_2_1 = 0 != *dest_2;
            dest_2 = &dest_2[1];
            i_1 -= 1;

            if (!flag_2_1)
                break;
        }

        if (~i - 1 > ~i_1 - 1)
        {
            char* dest_3 = source_1;
            int i_2 = 0xffffffff;

            while (i_2)
            {
                bool flag_4_1 = 0 != *dest_3;
                dest_3 = &dest_3[1];
                i_2 -= 1;

                if (!flag_4_1)
                    break;
            }

            int dest_4 = temp_1;
            int i_3 = 0xffffffff;

            while (i_3)
            {
                bool flag_5_1 = 0 != *dest_4;
                dest_4 += 1;
                i_3 -= 1;

                if (!flag_5_1)
                    break;
            }

            result = Camera_UpdateTransform_6(0, temp_1, ~i_3 - 1, temp_1 - (~i_2 - 1) + ~i_3 - 1, source_1);

            if (!result)
                return AnimTrigger_ParseConfigFile(*context);
        }
    }

    return result;
}
