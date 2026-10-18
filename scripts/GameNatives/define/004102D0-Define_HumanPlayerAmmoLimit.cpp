// IGI native provenance: Define_HumanPlayerAmmoLimit - 0x004102D0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004102D0-Define_HumanPlayerAmmoLimit.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004102D0-Define_HumanPlayerAmmoLimit.hpp"
int Define_HumanPlayerAmmoLimit(int context, int* arg2 )

// Sig: void Define_HumanPlayerAmmoLimit(...)
// Note: Registers an ammo-limit entry (max carried ammo class) for the human
// player during a humanplayer definition block (argc unknown - parser context).
{
    int i = 0;
    g_56df98 = 0;

    if (*arg2 > 0)
    {
        do
        {
            int* context_1 = (g_56df98 << 3) + &g_56df9c;
            *context_1 = Script_GetArg(arg2, i);
            int item_30 = i + 1;
            i += 2;
            context_1[1] = Script_GetArg(arg2, item_30);
            g_56df98 += 1;
        } while (i < *arg2);
    }

    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
