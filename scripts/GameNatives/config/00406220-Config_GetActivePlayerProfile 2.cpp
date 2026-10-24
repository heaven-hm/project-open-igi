// IGI native provenance: Config_GetActivePlayerProfile - 0x00406220 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00406220-Config_GetActivePlayerProfile.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_GetActivePlayerProfile.h"
void* Config_GetActivePlayerProfile()

{
// Sig: ProfileRecord* Config_GetActivePlayerProfile()
// Note: RETURNS pointer to the ACTIVE PROFILE's graphic-options/config record:
// computed as 0xBC2388 + 0xD14*[0xBC2384] (verified against retail
// defaults-reset fcn.00403B70 which walks the same layout). Takes no args.
// Despite the historical name this returns a record pointer, not a bool. Old
// analyst name IsPlayerProfileActive was wrong - it returns the record pointer,
// not a bool.
    return &(&g_bc2388)[g_bc2384 * 0x159];
}
