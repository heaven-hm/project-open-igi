// IGI native provenance: SplineObj_OnEvent - 0x00503070 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00503070-SplineObj_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00503070-SplineObj_OnEvent.hpp"
int* SplineObj_OnEvent(void* context, int* arg2)

{
    int* result = *(context + 0xf0);

    if (result)
    {
        double item_c_1 = result[0x3e] * 0.000244140625f;
        Game_DataSymbolLoad(0xa7a50c, "Segment length: %f meter\n");
        void* data_val = *(context + 0xf0);
        double item_c_2 = (*(data_val + 0x100) - *(data_val + 0xfc)) * 0.000244140625f;
        int item_10_1 = 0xa7a50c;
        Game_DataSymbolLoad(0xa7a50c, "%sModel length: %f meter\n");
        result = arg2;
        *result = 0xa7a50c;
    }

    return result;
}
