// IGI native provenance: Config_FillMissionTextBox - 0x00405760 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00405760-Config_FillMissionTextBox.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_FillMissionTextBox.h"
int Config_FillMissionTextBox(int context)

// Sig: void Config_FillMissionTextBox()
// Note: Fills the current text box with the selected mission's briefing text.
// Use inside the mission-briefing text control's script.
{
    void* data_1;

    for (int i = 1; i != 0xffffffff; i = *(data_1 + 0x4a4))
    {
        data_1 = FX_ExplosionSpawn_6(i);
        Menu_FormatScreen_7(data_1 + 0x24, i);
    }

    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
