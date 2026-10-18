// IGI native provenance: Mem_InitHeapState - 0x004B0BA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0BA0-Mem_InitHeapState.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Mem_InitHeapState.h"
int Mem_InitHeapState()

{
    g_a960f0 = &g_a960f4;
    g_a960f8 = &g_a960f0;
    g_a960f4 = 0;
    g_9422fc = 1;
    QHash_ValueSet();
    g_a960e0 = 0x10;
}
