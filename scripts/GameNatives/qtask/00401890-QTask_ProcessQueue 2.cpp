// IGI native provenance: QTask_ProcessQueue - 0x00401890 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401890-QTask_ProcessQueue.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "QTask_ProcessQueue.h"
int QTask_ProcessQueue(short context, int arg2)

{
    int result = arg2 * 6;
    *(result + &g_54f932) = 1;
    *(result + 0x54f930) = context;
    *(result + 0x54f933) = 0;
    return result;
}
