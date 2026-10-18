// IGI native provenance: HumanPlayer_CheckCollision_10 - 0x00413BD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00413BD0-HumanPlayer_CheckCollision_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_CheckCollision_10.h"
int* HumanPlayer_CheckCollision_10(int context)

{
    int data_val = Weapons_CountGet();
    int source_val = 0;

    if (data_val > 0)
    {
        do
        {
            int* result = HumanPlayer_TakeDamage_10(source_val);

            if (*result == context)
                return result;

            source_val += 1;
        } while (source_val < data_val);
    }

    return nullptr;
}
