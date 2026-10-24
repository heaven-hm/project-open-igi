// IGI native provenance: DynObj_UpdateDynTree_14 - 0x004CF690 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CF690-DynObj_UpdateDynTree_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CF690-DynObj_UpdateDynTree_14.hpp"
int* DynObj_UpdateDynTree_14(void* context)

{
    int* result = Mem_Alloc(0x14, 4);
    void* item_24 = context;
    char* item_28 = &g_a4ed6c;
    int context_val = 0;
    result[1] = 0;
    *result = 0;
    int data_1;
    void* entity_val;
    int temp_val;
    data_1 = Game_DataSymbolLoad(0xa4ec6c, "%smodels/%s.mef");
    int item_38 = 0xa4ec6c;
    int* data_2 = Resource_Load(data_1, temp_val, entity_val, nullptr);
    result[4] = data_2;
    int item_8;
    result[3] = Ilff_FindFormChunk(data_2, 0x534d4553, &item_8);
    void* temp_2 = result[4];
    result[2] = item_8 / 0x1c;
    void* data_5 = Ilff_FindFormChunk(temp_2, 0x53464143, nullptr);
    void* data_7 = Ilff_FindFormChunk(result[4], 0x53565458, nullptr);
    void* data_8 = Ilff_FindFormChunk(result[4], 0x45444745, nullptr);
    int i = 0;

    if (result[2] > 0)
    {
        do
        {
            void** entity_8 = result[3] + context_val;
            context_val += 0x1c;
            i += 1;
            *entity_8 = data_5 + *entity_8 * 0x1c;
            int temp_7 = entity_8[2];
            entity_8[1] = data_7 + entity_8[1] * 0xc;
            entity_8[2] = data_8 + (temp_7 << 3);
        } while (i < result[2]);
    }

    return result;
}
