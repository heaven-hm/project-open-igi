// IGI native provenance: Model_GetAttachmentCount - 0x004D0D10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D0D10-Model_GetAttachmentCount.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Model_GetAttachmentCount.h"
int Model_GetAttachmentCount(void* context)

{
    return *(*(*(context + 0xc) + 0xc) + 0x7e);
}
