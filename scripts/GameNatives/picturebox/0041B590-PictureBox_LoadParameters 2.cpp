// IGI native provenance: PictureBox_LoadParameters - 0x0041B590 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041B590-PictureBox_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PictureBox_LoadParameters.h"
int* PictureBox_LoadParameters(long double context , void* arg2)

{
    int* result = Script_FormatString_6(arg2 + 0x9ac);

    if (result)
    {
        Script_SpawnNewTask_7(arg2 + 0x9ac, arg2);
        int entity_1 = *(arg2 + 0x958);
        result = nullptr;

        if (entity_1 > 0)
        {
            int* temp_1 = arg2 + 0x94;

            while (context != *temp_1)
            {
                result += 1;
                temp_1 = &temp_1[0x12];

                if (result >= entity_1)
                    return result;
            }

            *(arg2 + 0x954) = result;
        }
    }

    return result;
}
