// IGI native provenance: SetWindowHandle - 0x0048F0E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048F0E0-SetWindowHandle.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048F0E0-SetWindowHandle.hpp"
extern "C" {
extern void *g_5c8bc4;
}
void *SetWindowHandle(void *window)
{
    g_5c8bc4 = window;
    return window;
}
