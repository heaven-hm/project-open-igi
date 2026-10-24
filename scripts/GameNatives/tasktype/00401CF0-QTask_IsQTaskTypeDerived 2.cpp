// IGI native provenance: QTask_IsQTaskTypeDerived - 0x00401CF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401CF0-QTask_IsQTaskTypeDerived.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "QTask_IsQTaskTypeDerived.h"
int QTask_IsQTaskTypeDerived(int context, short arg2)

// Sig: bool QTask_IsQTaskTypeDerived(unsigned short typeId, unsigned short baseTypeId)
// Note: Checks equality and walks the task-type parent chain until the sentinel
// type. Ghidra/r2 CFG agreement: 5 basic blocks and 6 edges; see
// assets/IGI1-Native-Name-Evidence.json.
{
    int result = context;

    while (result != arg2)
    {
        result = (result & 0xffff) * 0x30;
        result = *(result + &g_af5ee8);

        if (result == 0x181)
        {
            result = 0;
            return result;
        }
    }

    result = 1;
    return result;
}
