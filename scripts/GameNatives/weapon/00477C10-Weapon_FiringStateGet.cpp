// IGI native provenance: Weapon_FiringStateGet - 0x00477C10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00477C10-Weapon_FiringStateGet.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00477C10-Weapon_FiringStateGet.hpp"
int Weapon_FiringStateGet()

// Sig: unsigned char Weapon_FiringStateGet(void)
// Note: Returns the byte at the retail weapon firing-state configuration global
// 0x005407B9. Ghidra/r2 CFG agreement: 1 basic block and 0 edges; see
// assets/IGI1-Native-Name-Evidence.json.
{
    int result;
    result = g_5407b9;
    return result;
}
