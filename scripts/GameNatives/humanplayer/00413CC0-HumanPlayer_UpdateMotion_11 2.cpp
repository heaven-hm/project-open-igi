// IGI native provenance: HumanPlayer_UpdateMotion_11 - 0x00413CC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00413CC0-HumanPlayer_UpdateMotion_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_UpdateMotion_11.h"
int* HumanPlayer_UpdateMotion_11(int* context, int arg2)

{
    int* data_val = context;
    int* i = data_val[2];

    if (*i && i)
    {
        do
        {
            int entity_val;
            int temp_val;
            data_val = HumanPlayer_LoadParameters_13(data_val, temp_val, entity_val, i);

            if (data_val && *data_val == arg2)
                return i;

            i = *i;

            if (!i)
                break;

            if (!*i)
                break;
        } while (i);
    }

    return 0;
}
