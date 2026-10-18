// IGI native provenance: AI_CheckHearing_5 - 0x00453880 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00453880-AI_CheckHearing_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckHearing_5.h"
int* AI_CheckHearing_5(int* context, int arg2)

{
    int* data_1 = *(*context + 8);
    int source_val = *data_1;
    int source_1 = -(source_val);
    int dest_val = 0;
    int* result = (source_1 - source_1) & data_1;

    if (result)
    {
        while (dest_val < arg2)
        {
            int data_2 = PatrolPathCommand_Process(result);

            if (data_2 >= 2)
            {
                if (data_2 <= 3)
                    context[3] = result;
                else if (data_2 == 6)
                    context[2] = result;
            }

            int* data_3 = *result;

            if (!data_3)
                result = nullptr;
            else
            {
                int source_3 = *data_3;
                int source_4 = -(source_3);
                result = (source_4 - source_4) & data_3;
            }

            dest_val += 1;

            if (!result)
                break;
        }
    }

    return result;
}
