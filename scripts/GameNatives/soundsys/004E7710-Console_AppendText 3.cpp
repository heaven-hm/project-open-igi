// IGI native provenance: Console_AppendText - 0x004E7710 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E7710-SoundSys_SetMasterVolume_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004E7710-Console_AppendText.hpp"
void Console_AppendText(int* context, char* arg2)

{
    int context_val;
    int item_4 = context_val;

    if (!*context)
        return;

    char* source_1;

    for (char* i = arg2; i; i = i + source_1 + 1)
    {
        char j = *i;
        char context_1 = 0;
        source_1 = nullptr;

        while (j)
        {
            if (j == 0xa)
            {
                *(source_1 + i) = 0;
                context_1 = 1;
                break;
            }

            j = *(source_1 + i + 1);
            source_1 = &source_1[1];
        }

        Sys_AllocateMemory_21(*context, i, context[6], context[7]);

        if (!context_1)
        {
            int j_1 = 0xffffffff;

            while (j_1)
            {
                bool flag_0_1 = 0 != *i;
                i = &i[1];
                j_1 -= 1;

                if (!flag_0_1)
                    break;
            }

            SoundSys_LoadSoundDef_10(context, ~j_1 - 1);
            break;
        }

        SoundSys_SetFrequency_10(context, 1);
    }
}
