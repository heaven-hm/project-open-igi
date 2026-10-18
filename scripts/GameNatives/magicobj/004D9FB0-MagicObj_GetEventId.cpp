// IGI native provenance: MagicObj_GetEventId - 0x004D9FB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D9FB0-MagicObj_GetEventId.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D9FB0-MagicObj_GetEventId.hpp"
int MagicObj_GetEventId()

// Sig: unsigned char MagicObj_GetEventId(void)
// Note: Returns the MagicObj event-slot byte stored at 0x0054864A, allocated by
// MagicObj_Initialize and released by MagicObj_Shutdown. Ghidra/r2: exact
// boundary, 1 basic block, 0 CFG edges, cdecl no-argument getter.
{
    int result;
    result = g_54864a;
    return result;
}
