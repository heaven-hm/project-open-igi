// IGI native provenance: Debug_Error - 0x004AF7B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AF7B0-Debug_Error.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AF7B0-Debug_Error.hpp"
extern "C" {
extern void *g_936268;
}
void Debug_Error(const char *message)
{
    if (!g_936268)
        return;
    Debug_ErrorFatal("Fatal error:\n\n", message);
}
