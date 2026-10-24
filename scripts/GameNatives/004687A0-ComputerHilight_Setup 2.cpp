// IGI native provenance: ComputerHilight_Setup - 0x004687A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004687A0-ComputerHilight_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "ComputerHilight_Setup.h"
int ComputerHilight_Setup(long double context , int* arg2, void* arg3)

{
    int* dest_val = arg2;
    Script_SpawnNewTask_7(&dest_val[0xe], dest_val);
    int result = context;

    if (result)
    {
        int flagP1 = 0;
        *(arg3 + 0x18) = dest_val[0xa4];
        *(arg3 + 0x1c) = dest_val[0xad];
        *(arg3 + 0x20) = dest_val[0xb6];
        *(arg3 + 0x24) = &dest_val[0xb7];
        *(arg3 + 0x28) = &dest_val[0xf7];
        result = dest_val[0x63];

        if (result > 0)
        {
            arg2 = &dest_val[0x64];

            do
            {
                void* data_3 = Script_ExecuteLevelScript_4(*arg2);

                if (data_3)
                {
                    unsigned int data_5 = AI_CheckHearing_25();
                    int entity_4 = 0;
                    entity_4 = *(data_3 + 0x1c);
                    int data_9 = *(((data_5 * 0x180 + entity_4) << 2) + &g_a96ae0);

                    if (data_9)
                        data_9(data_3, arg3);
                }

                result = dest_val[0x63];
                flagP1 += 1;
                arg2 = &arg2[1];
            } while (flagP1 < result);
        }
    }

    return result;
}
