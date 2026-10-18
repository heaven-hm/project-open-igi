// IGI native provenance: RenderLight_ProcessStep_52B4D0 - 0x0052B4D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052B4D0-RenderLight_ProcessStep_52B4D0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0052B4D0-RenderLight_ProcessStep_52B4D0.hpp"
int* RenderLight_ProcessStep_52B4D0(int arg1)

{
    int a843ac_1 = g_a843ac;
    int context_val = 0;

    if (a843ac_1 == arg1)
    {
        g_a843ac = 0;
        return a843ac_1;
    }

    int* result_val = &g_a843ac;
    int edi_1;

    do
    {
        edi_1 = result_val[1];
        result_val = &result_val[1];
        context_val += 1;
    } while (edi_1 != arg1);
    (&g_a843ac)[context_val] = 0;
    return result_val;
}
