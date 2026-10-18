// IGI native provenance: Menu_FormatScreen_2 - 0x00418790 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00418790-Menu_FormatScreen_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_FormatScreen_2.h"
int Menu_FormatScreen_2(void* context)

{
    int context_val = *(context + 0x27e4);
    void* dest_val = *(context + 0x30);
    int data_val = Menu_HandleInput_13();
    int entity_val = 0;
    entity_val = *(dest_val + 0x1c);
    int data_4 = *(((data_val * 0x180 + entity_val) << 2) + &g_a96ae0);

    if (data_4)
        data_4(dest_val);

    int result = Config_SaveGraphicOptions_2();
    *(context + 0x27e4) = result;

    if (result == context_val)
        return result;

    return Menu_FormatScreen(context, 1);
}
