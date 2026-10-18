// IGI native provenance: QTask_CleanupTypes_2 - 0x00401D30 - CFG score 72.2% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00401D30-QTask_CleanupTypes_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00401D30-QTask_CleanupTypes_2.hpp"
int QTask_CleanupTypes_2(char context, int arg2)

{
    *(arg2 + &g_54ff24) = context;
    *(context + 0xa969e0) = arg2;
    /* tailcall */
    return QTask_DeleteTask();
}
