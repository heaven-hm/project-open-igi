// IGI native provenance: Screen_Process - 0x004E7540 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004E7540-Screen_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Screen_Process.hpp"
int Screen_Process(int context, char arg2)

{
    if (g_a5ea65)
        return 0;

    memcpy(0xa5e620, context, 0x11c);
    g_a5ea64 = arg2;
    g_a5ea65 = 1;
    return 1;
}
