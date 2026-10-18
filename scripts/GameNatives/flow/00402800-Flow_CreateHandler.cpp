// IGI native provenance: Flow_CreateHandler - 0x00402800 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00402800-Flow_CreateHandler.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Flow_CreateHandler.hpp"
struct status_msg* Flow_CreateHandler(void* context)

{
    int kind;
    kind = g_567c7c;
    return QTask_HandleInput(context, kind, 0);
}
