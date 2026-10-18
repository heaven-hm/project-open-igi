// IGI native provenance: Env_CheckAmbientArea_12 - 0x0044C8C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044C8C0-Env_CheckAmbientArea_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044C8C0-Env_CheckAmbientArea_12.hpp"
void* Env_CheckAmbientArea_12()

{
    int* entity_val = **&g_5bda60;
    int data_val = *entity_val;
    int data_1 = -(data_val);
    return (data_1 - data_1) & entity_val;
}
