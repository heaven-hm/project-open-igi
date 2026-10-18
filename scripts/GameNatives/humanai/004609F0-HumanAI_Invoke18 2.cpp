// IGI native provenance: HumanAI_Invoke18 - 0x004609F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004609F0-HumanAI_Invoke18.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_Invoke18.h"

int HumanAI_Invoke18(int arg1, bool* arg2)

{
    *arg2 = *(arg1 + 0xfc) == 1;
    return arg1;
}
