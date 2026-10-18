// IGI native provenance: Menu_UpdateLanguage_10 - 0x00420240 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00420240-Menu_UpdateLanguage_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_UpdateLanguage_10.hpp"
void* Menu_UpdateLanguage_10(void* context, int arg2, int arg3, int arg4 , int arg5)

{
    *(context + 0x20) = arg2;
    *(context + 0x24) = arg3;
    void* temp_1 = *(context + 0x9c);
    *(context + 0x2c) = arg4;
    *(context + 0xac) = arg4 - *(temp_1 + 0x2c) - *(*(context + 0x98) + 0x2c) - 6;
    *(context + 0x248) = arg5;
    return context;
}
