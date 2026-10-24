// IGI native provenance: DynObj_LinkDynamicCube_7 - 0x004C67E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C67E0-DynObj_LinkDynamicCube_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C67E0-DynObj_LinkDynamicCube_7.hpp"
int DynObj_LinkDynamicCube_7(int context, int arg2, int arg3, int arg4, int arg5, int arg6)

{
    int data_3 = (context ^ arg4) | (arg2 ^ arg5) | (arg3 ^ arg6);

    if (!data_3)
        return 0x2000;

    int eflags;
    int data_5;
    data_5 = __bsr_gprv_memv(data_3);
    int result = 1 << (data_5 & 0x1f);

    if (result < 0x2000)
        return 0x2000;

    if (result <= 0x40000000)
        return result;

    return 0x40000000;
}
