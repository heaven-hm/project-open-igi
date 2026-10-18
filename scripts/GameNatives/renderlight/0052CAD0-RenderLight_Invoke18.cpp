// IGI native provenance: RenderLight_Invoke18 - 0x0052CAD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052CAD0-RenderLight_Invoke18.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0052CAD0-RenderLight_Invoke18.hpp"
int RenderLight_Invoke18(void* arg1)

{
    int result = *(arg1 + 0x78);

    if (result)
        result = Sys_CloseFileStream_18(result);

    return result;
}
