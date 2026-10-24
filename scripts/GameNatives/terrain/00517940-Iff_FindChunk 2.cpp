// IGI native provenance: Iff_FindChunk - 0x00517940 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00517940-Iff_FindChunk.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Iff_FindChunk.h"
void** Iff_FindChunk(void* context, int arg2)

{
    for (void** i = *(context + 8); i; i = *i)
    {
        int data_val = i[4];

        if (data_val == 0x464f524d)
        {
            void** data_1 = Iff_FindChunk(i, arg2);

            if (data_1)
                return data_1;
        }
        else if (data_val == arg2)
            return i;
    }

    return nullptr;
}
