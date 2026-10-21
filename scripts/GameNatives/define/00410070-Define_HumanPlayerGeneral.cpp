// IGI native provenance: DefineHumanPlayerGeneral - 0x00410070 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00410070-Define_HumanPlayerGeneral.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00410070-Define_HumanPlayerGeneral.hpp"
extern "C" {
extern float g_56e1e8[8];
}
void DefineHumanPlayerGeneral(void *context)
{
    float first = Qvm_GetRealArgument(context, 0);
    g_56e1e8[0] = first;
    float second = Qvm_GetRealArgument(context, 1) * 4096000.0f;
    float third = Qvm_GetRealArgument(context, 2) * 9.259259059035685e-06f;
    g_56e1e8[1] = third;
    float fourth = Qvm_GetRealArgument(context, 3) * 4096000.0f;
    float fifth = Qvm_GetRealArgument(context, 4) * 9.259259059035685e-06f;
    g_56e1e8[2] = fifth;
    float sixth = Qvm_GetRealArgument(context, 5) * 4096.0;
    g_56e1e8[3] = sixth;
}
