// IGI native provenance: DynObj_CreateMagicObject_12 - 0x004CE830 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CE830-DynObj_CreateMagicObject_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_CreateMagicObject_12.h"
int* DynObj_CreateMagicObject_12(int* context, int arg2)

{
    int* result = context;
    int entity_2 = result[0x37a];
    int temp_1 = -(result[0x379]);
    int temp_2 = 0;
    double item_58 = -(result[0x378]);
    double item_50 = temp_1;
    double item_48 = -(entity_2);

    if (!arg2)
    {
        result[0x37c] = 1;

        if (g_54876c)
        {
            int context_val;
            int item_60_1 = context_val;
            void* flagP1 = result[4];
            int item_40;
            memcpy(&item_40, flagP1, 0x40);
            char item_10_1 = 0;
            int i = 7;
            void item_34;
            int* source_2 = &item_34;
            int* dest_1 = flagP1 + 0x28;

            do
            {
                int entity_6 = *dest_1;
                dest_1 -= 4;
                *source_2 = entity_6;
                source_2 = &source_2[1];
                char entity_9 = item_10_1 | (*(flagP1 + 0x30) >> (temp_2 & 0x1f) & 1) << (i & 0x1f);
                temp_2 += 1;
                i -= 1;
                item_10_1 = entity_9;
            } while (i > 0xffffffff);

            SoundSys_AttachLipsync_3();
            result = DynObj_ParseConfigFile_12(context, &item_40, 0, &item_58);
            SoundSys_EvaluateTrigger_3();
        }
    }

    return result;
}
