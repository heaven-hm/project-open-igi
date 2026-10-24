// IGI native provenance: FX_FragmentSimulate_16 - 0x0048A620 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048A620-FX_FragmentSimulate_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_FragmentSimulate_16.h"
int FX_FragmentSimulate_16(int* context, int arg2)

{
    int entity_1 = context[1] + arg2;
    context[1] = entity_1;
    int f0 = context[4] * entity_1 / *context;
    int i = 0;

    if (context[5] > 0)
    {
        do
        {
            int entity_5;
            int dest_2;

            if (f0 > 0)
                dest_2 = __memfill_u32(context[6] + ((context[4] * i) << 2), 0xff00, f0);
            i += 1;
        } while (i < context[5]);
    }

    void item_28;
    memset(&item_28, 0, 0x28);
    int item_24 = Display_CreateSurface_4();
    int data_9 = context[4];
    int item_8 = context[6];
    int item_10 = data_9;
    int item_18 = context[2];
    int item_1c = data_9 << 2;
    int item_14 = context[3];
    int item_c = context[5];
    int item_4 = 1;
    return Display_RestoreDevice_4(&item_28);
}
