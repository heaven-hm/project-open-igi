// IGI native provenance: HumanAI_Invoke005011 - 0x004645B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004645B0-HumanAI_Invoke005011.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_Invoke005011.hpp"

int* HumanAI_Invoke005011(int* arg1)

{
    int* result = arg1;

    if (result[0x36])
        result = AI_005011(result);

    return result;
}
