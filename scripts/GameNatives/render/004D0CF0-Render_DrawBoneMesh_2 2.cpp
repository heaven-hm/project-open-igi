// IGI native provenance: Model_GetAttachment - 0x004D0CF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D0CF0-Model_GetAttachment.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Model_GetAttachment.h"
int Model_GetAttachment(void* context, int arg2)

{
    return *(*(*(context + 0xc) + 0xc) + 0x8c) + arg2 * 0x44;
}
