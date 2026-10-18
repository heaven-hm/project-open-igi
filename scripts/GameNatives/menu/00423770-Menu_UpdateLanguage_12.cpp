// IGI native provenance: Menu_UpdateLanguage_12 - 0x00423770 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00423770-Menu_UpdateLanguage_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_UpdateLanguage_12.hpp"
void* Menu_UpdateLanguage_12(void* context, void* arg2, int arg3)

{
    void* source_val = context;
    int* dest_val = *(source_val + (arg2 << 2) + 0x154);
    *(source_val + (arg2 << 2) + 0x254) = arg3;

    if (!dest_val)
        return arg2;

    KEY_ANGLE(arg3, nullptr, &context);
    return Sys_FreeMemory_18(dest_val, Menu_LocalSS(*(source_val + 0x44), context));
}
