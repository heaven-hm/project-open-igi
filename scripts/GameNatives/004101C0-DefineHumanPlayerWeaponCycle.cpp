// IGI native provenance: Define_HumanPlayerWeaponCycle - 0x004101C0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004101C0-Define_HumanPlayerWeaponCycle.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Define_HumanPlayerWeaponCycle.h"
int Define_HumanPlayerWeaponCycle(int context, int* arg2 )

// Sig: void Define_HumanPlayerWeaponCycle(...)
// Note: Collects the weapon-cycle list for the human player: loops the parsed
// entries and stores them into the cycle table at 0x56DF1C with count at
// 0x56DF18 (verified in igi.exe). Call within a humanplayer weapon-cycle
// definition block; argc unknown (parser context).
{
    int i = 0;
    g_56df18 = 0;

    if (*arg2 > 0)
    {
        do
        {
            int i_1 = i;
            i += 1;
            *((g_56df18 << 2) + &g_56df1c) = Script_GetArg(arg2, i_1);
            g_56df18 += 1;
        } while (i < *arg2);
    }

    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
