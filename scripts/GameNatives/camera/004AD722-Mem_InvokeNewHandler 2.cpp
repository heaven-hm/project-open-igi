// IGI native provenance: Mem_InvokeNewHandler - 0x004AD722 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD722-Mem_InvokeNewHandler.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AD722-Mem_InvokeNewHandler.hpp"
int Mem_InvokeNewHandler(int context)

{
    int data_3 = g_936254;

    if (data_3 && data_3(context))
        return 1;

    return 0;
}
