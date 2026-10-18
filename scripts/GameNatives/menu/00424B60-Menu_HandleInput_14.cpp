// IGI native provenance: Menu_HandleInput_14 - 0x00424B60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00424B60-Menu_HandleInput_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_HandleInput_14.hpp"
struct status_msg* Menu_HandleInput_14(void* context, int arg2)

{
    return QTask_HandleInput(context, Menu_PlayMusic_13(), arg2);
}
