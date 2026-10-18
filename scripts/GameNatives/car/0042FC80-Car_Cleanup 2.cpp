// IGI native provenance: Car_Cleanup - 0x0042FC80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0042FC80-Car_Cleanup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Car_Cleanup.h"
int* Car_Cleanup(void* context, int arg2)

{
    int* result;
    result = *(context + 0x2ed5);

    if (!result)
    {
        (*(((DynObj_LinkDynamicCube_12() * 0x180 + Script_ExecuteLevelScript_16()) << 2) + &g_a96ae0))(context, arg2);
        result = *(context + 0x748);

        if (result)
        {
            result = QTask_FindType(result);
            *(context + 0x748) = 0;
        }
    }

    return result;
}
