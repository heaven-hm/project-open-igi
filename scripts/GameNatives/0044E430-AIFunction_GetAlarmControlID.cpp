// IGI native provenance: AIFunction_GetAlarmControlID - 0x0044E430 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0044E430-AIFunction_GetAlarmControlID.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044E430-AIFunction_GetAlarmControlID.hpp"
int AIFunction_GetAlarmControlID(int context)

// Sig: int AIFunction_GetAlarmControlID()
// Note: RETURNS the bound alarm-control id. Takes no args.
{
    int data_1 = *(g_5bda58 + 0x3ac4);
    int item_18 = data_1;
    char* item_8 = &g_567c74;
    double item_10 = data_1;
    memcpy(context, &item_18, 0x18);
    return context;
}
