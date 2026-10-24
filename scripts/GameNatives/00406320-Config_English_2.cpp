// IGI native provenance: Config_English_2 - 0x00406320 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00406320-Config_English_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00406320-Config_English_2.hpp"
void* Config_English_2(int context) {
    return &"ENGLISH"[context * 2];
}
