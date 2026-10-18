// IGI native provenance: HumanPlayer_Invoke8 - 0x004108B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004108B0-HumanPlayer_Invoke8.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_Invoke8.h"

int* HumanPlayer_Invoke8(int* arg1)

{
    *arg1 = g_HumanPlayerFlags;
    return arg1;
}
