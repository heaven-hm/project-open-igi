// IGI native provenance: QTask_RegisterEventCallback_3 - 0x00401FA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401FA0-QTask_RegisterEventCallback_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00401FA0-QTask_RegisterEventCallback_3.hpp"
unsigned int QTask_RegisterEventCallback_3(short context, int arg2)

{
    unsigned int result = context;
    *(result * 0x30 + 0xaf5f0c) = arg2 | 0x20000000;
    return result;
}
