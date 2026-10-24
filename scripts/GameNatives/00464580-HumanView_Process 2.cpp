// IGI native provenance: HumanView_Process - 0x00464580 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00464580-HumanView_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanView_Process.h"
int HumanView_Process(void* context, int* arg2)

{
    char result = *(context + 0xd9);

    if (result)
    {
        Render_DrawSplineMesh_12(context);
        result = DynObj_RegisterSymbol_9(*arg2, context);
        *(context + 0xd9) = 0;
    }

    return result;
}
