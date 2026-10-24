// IGI native provenance: Human_GetLimbDamageRecord - 0x00489E10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489E10-FX_FragmentSimulate_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00489E10-Human_GetLimbDamageRecord.hpp"
int Human_GetLimbDamageRecord(void* context, int arg2)

{
    return (arg2 << 4) + *(context + 8);
}
