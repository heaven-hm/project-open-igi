// IGI native provenance: Ilff_FindFormChunk - 0x004CEAA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CEAA0-Ilff_FindFormChunk.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CEAA0-Ilff_FindFormChunk.hpp"
void* Ilff_FindFormChunk(void* context, int arg2, int* arg3)

{
    int data_val = *(context + 4);
    int* entity_val = context + 0xc;
    int data_2 = *entity_val;

    if (arg2
        != (((data_2 & 0xff0000) | data_2 >> 0x10) >> 8 | ((data_2 & 0xff00) | data_2 << 0x10) << 8))
    {
        int data_6;

        do
        {
            int data_4 = entity_val[1];
            entity_val = entity_val + (((((data_4 & 0xff0000) | data_4 >> 0x10) >> 8
                | ((data_4 & 0xff00) | data_4 << 0x10) << 8) + 3) & 0xfffffffc) + 8;

            if (entity_val >= (((data_val & 0xff0000) | data_val >> 0x10) >> 8 | (data_val << 0x10 | (data_val & 0xff00)) << 8)
                    + context + 8)
                return 0;

            data_6 = *entity_val;
        } while (arg2 !=
            (((data_6 & 0xff0000) | data_6 >> 0x10) >> 8 | ((data_6 & 0xff00) | data_6 << 0x10) << 8));
    }

    if (arg3)
    {
        int data_8 = entity_val[1];
        *arg3 = ((data_8 & 0xff0000) | data_8 >> 0x10) >> 8 | ((data_8 & 0xff00) | data_8 << 0x10) << 8;
    }

    return &entity_val[2];
}
