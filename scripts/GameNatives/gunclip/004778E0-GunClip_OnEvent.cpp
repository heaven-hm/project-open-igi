// IGI native provenance: GunClip_OnEvent - 0x004778E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004778E0-GunClip_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004778E0-GunClip_OnEvent.hpp"
int GunClip_OnEvent(void* context, int arg2)

{
    void* dest_val = *(*(context + 0x14) + 0x20);
    int result = SoundSys_SetFrequency_9();

    if (*(dest_val + 0x138) < result - 1)
        *(context + 0x13c) = 0xffffffff;
    else
    {
        if (*(context + 0x13c) == 0xffffffff)
            *(context + 0x13c) = result;

        result -= *(context + 0x13c);

        if (!(result & 1))
        {
            unsigned int data_2 = Sys_FreeMemory_14(Level_StartMission_5(), 3);
            int item_18 = 0;
            int item_14 = 4;
            int item_10 = 7;
            Sys_FreeMemory_12(context + 0x70, context + 0x70, (&item_18)[data_2] * 0.0174532924f);
            int item_c = arg2;
            int item_8 = arg2 + 0x18;
            void* item_4 = context;
            unsigned int data_6 = MagicObj_GetEventId();
            int data_7 = 0;
            data_7 = *(context + 0x1c);
            int data_8 = *(((data_6 * 0x180 + data_7) << 2) + &g_a96ae0);

            if (data_8)
                data_8(context, &item_c);

            return (*((DynObj_LookupHashItem_5() << 2) + &g_a982e0))(context, 0);
        }
    }

    return result;
}
