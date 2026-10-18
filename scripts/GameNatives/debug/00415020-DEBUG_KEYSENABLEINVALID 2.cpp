// IGI native provenance: DEBUG_KEYSENABLEINVALID - 0x00415020 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00415020-DEBUG_KEYSENABLEINVALID.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DEBUG_KEYSENABLEINVALID.h"
int DEBUG_KEYSENABLEINVALID(void* context)

// Sig: void DEBUG_KEYSENABLEINVALID()
// Note: TOMBSTONE - DO NOT INVOKE. The retail debug-keys native address is
// UNKNOWN. The old value 0x0041502 lies outside .text (0x401000-0x532510) and
// would crash on invoke; no verified replacement found in igi.exe (unknown >
// fabricated per the local reverse-engineering acceptance policy). This entry
// exists only so JSON consumers see the warning instead of silently missing the
// key.
{
    if (*(context + 0xc28c62) & 0x80 && !(*(context + 0xc28d62) & 0x80) && g_c28c7f & 0x80
            && g_c28c8c & 0x80 && Config_GetActivePlayerProfileRecord()
            && *(Config_GetActivePlayerProfileRecord() + 0x560) >= 0xe && g_57b194)
        return 1;

    return 0;
}
