// IGI native provenance: DynObj_LoadPackageLovs - 0x004CF630 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CF630-DynObj_LoadPackageLovs.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CF630-DynObj_LoadPackageLovs.hpp"
int* DynObj_LoadPackageLovs(void* context)

{
    int* result = Ilff_FindFormChunk(*(context + 8), 0x53564f4c, nullptr);

    if (result)
    {
        int i_1 = *result;
        void* source_1 = &result[1];

        if (i_1 > 0)
        {
            int i;

            do
            {
                HeapList_Insert(context + 0x1c, DynObj_UpdateDynTree_14(source_1));
                void* dest_1 = source_1;
                int j = 0xffffffff;
                result = nullptr;

                while (j)
                {
                    bool flag_0_1 = 0 != *dest_1;
                    dest_1 += 1;
                    j -= 1;

                    if (!flag_0_1)
                        break;
                }

                i = i_1;
                i_1 -= 1;
                source_1 = source_1 + ~j - 1 + 1;
            } while (i != 1);
        }
    }

    return result;
}
