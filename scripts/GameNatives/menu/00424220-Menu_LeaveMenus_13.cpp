// IGI native provenance: Menu_LeaveMenus_13 - 0x00424220 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00424220-Menu_LeaveMenus_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_LeaveMenus_13.hpp"
void* Menu_LeaveMenus_13(void* context, void* arg2)

{
    int data_val = Menu_UpdateLanguage_3();
    void* source_val = arg2;

    while (*(source_val + 0x1c) != data_val)
    {
        source_val = *(source_val + 0x14);

        if (!source_val)
        {
            *(context + 0x44) = 0;
            return data_val;
        }

        data_val = Menu_UpdateLanguage_3();
    }

    *(context + 0x44) = source_val;
    return context;
}
