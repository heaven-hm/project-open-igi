// IGI native provenance: Resource_FileExists - 0x004B5B60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B5B60-Resource_FileExists.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Resource_FileExists.h"
int Resource_FileExists(void* context)

{
    if (!Is_ResourceLoaded(context, nullptr))
        return Sys_DispatchOpcode_7(context);

    return 1;
}
