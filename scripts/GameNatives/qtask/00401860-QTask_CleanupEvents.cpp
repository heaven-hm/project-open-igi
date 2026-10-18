// IGI native provenance: QTask_CleanupEvents - 0x00401860 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401860-QTask_CleanupEvents.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00401860-QTask_CleanupEvents.hpp"
unsigned int QTask_CleanupEvents(short context, char arg2)

{
    unsigned int result = arg2 * 6;
    *(result + &g_54f932) = 1;
    *(result + 0x54f930) = context;
    *(result + 0x54f933) = 1;
    return result;
}
