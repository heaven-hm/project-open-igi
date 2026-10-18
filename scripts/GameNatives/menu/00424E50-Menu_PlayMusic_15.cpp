// IGI native provenance: Menu_PlayMusic_15 - 0x00424E50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00424E50-Menu_PlayMusic_15.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_PlayMusic_15.hpp"
int Menu_PlayMusic_15(void* context, int* arg2, int* arg3, int* arg4, int* arg5)

{
    *arg2 = *(context + 0x24);
    *arg3 = *(context + 0x28);
    *arg4 = *(context + 0x3c);
    int result = *(context + 0x38);
    *arg5 = result;
    return result;
}
