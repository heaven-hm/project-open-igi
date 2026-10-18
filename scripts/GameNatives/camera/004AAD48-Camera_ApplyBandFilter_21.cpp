// IGI native provenance: Camera_ApplyBandFilter_21 - 0x004AAD48 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AAD48-Camera_ApplyBandFilter_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AAD48-Camera_ApplyBandFilter_21.hpp"
int Camera_ApplyBandFilter_21(int context)

{
    int result = HeapAlloc(g_c32428, HEAP_NONE, 0x140);
    g_c321f8 = result;

    if (!result)
        return result;

    g_c321f0 = 0;
    g_c321f4 = 0;
    g_c321ec = result;
    g_c321fc = context;
    g_c321e4 = 0x10;
    return 1;
}
