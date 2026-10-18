// IGI native provenance: HumanPlayer_ApplyRecoil_9 - 0x00413A60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00413A60-HumanPlayer_ApplyRecoil_9.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_ApplyRecoil_9.hpp"
int HumanPlayer_ApplyRecoil_9(void* context, int arg2)

{
    *(context + 0xd5c) = arg2;
    return arg2;
}
