// IGI native provenance: HumanPlayer_UpdateHealth - 0x00408390 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00408390-HumanPlayer_UpdateHealth.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_UpdateHealth.hpp"
int HumanPlayer_UpdateHealth(int context)

{
    int dest_val = 0;
    int result;

    for (void* i = &g_569210; i < 0x56df10; )
    {
        result = DynObj_LinkDynamicCube_11(context, dest_val, i);
        i += 0x268;
        dest_val += 1;
    }

    return result;
}
