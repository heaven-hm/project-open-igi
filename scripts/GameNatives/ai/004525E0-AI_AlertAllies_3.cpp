// IGI native provenance: AI_AlertAllies_3 - 0x004525E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004525E0-AI_AlertAllies_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AlertAllies_3.hpp"
void* AI_AlertAllies_3(int context)

{
    void* source_val = *(context + 0x14);
    short data_val = AI_UpdateOrders_16();
    char data_1;

    if (*(source_val + 0x1c) != data_val)
    {
        short data_2 = AI_SelectTarget_12();

        if (*(source_val + 0x1c) != data_2)
        {
            short data_3 = AI_AlertAllies_7();

            if (*(source_val + 0x1c) != data_3)
            {
                short data_4 = AI_FireWeapon_9();

                if (*(source_val + 0x1c) != data_4)
                    data_1 = context;
                else
                    data_1 = *(source_val + 0xd00);
            }
            else
                data_1 = *(source_val + 0xd00);
        }
        else
            data_1 = *(source_val + 0xd00);
    }
    else
        data_1 = *(source_val + 0xd00);

    int entity_val = 0;
    entity_val = !data_1;
    int item_c = entity_val;
    return AI_WriteChannel(context, 0, entity_val);
}
