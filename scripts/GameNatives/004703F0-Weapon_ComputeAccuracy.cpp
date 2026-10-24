// IGI native provenance: Weapon_ComputeAccuracy - 0x004703F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004703F0-Weapon_ComputeAccuracy.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004703F0-Weapon_ComputeAccuracy.hpp"
unsigned int Weapon_ComputeAccuracy()

{
    int data_val;
    data_val = g_53fec0;
    return QTask_CloseEvent_2(data_val);
}
