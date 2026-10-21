// IGI native provenance: Qvm_ReportError - 0x004B7E10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7E10-Qvm_ReportError.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7E10-Qvm_ReportError.hpp"
void Qvm_ReportError(void *context, int arg2)
{
    void *report = g_a96598;
    g_a965a8 = 1;
    if (!report)
        return;
    Qvm_ReportErrorTo(report, arg2);
}
