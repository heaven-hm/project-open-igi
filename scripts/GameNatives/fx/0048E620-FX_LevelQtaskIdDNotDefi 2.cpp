// IGI native provenance: FX_LevelQtaskIdDNotDefi - 0x0048E620 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E620-FX_LevelQtaskIdDNotDefi.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_LevelQtaskIdDNotDefi.h"
int* FX_LevelQtaskIdDNotDefi(int* context, void* const arg2)

{
    int entity_val;
    int item_4_1 = entity_val;
    void* const data_val = arg2;
    void* source_val = *(data_val + 0x28);
    int* dest_val = context;
    *(data_val + 0x18) = FX_DebrisSpawn_20;
    *(data_val + 0x20) = FX_TrailUpdate_20;
    char context_val = 0;
    int item_4 = 0;

    if (*(source_val + 8) == dest_val[0xeb0])
    {
        int data_2 = Script_ExecuteLevelScript_4(dest_val[0xeb1]);
        item_4 = data_2;

        if (!data_2)
        {
            int item_18_2 = dest_val[0xeb1];
            int data_3 = Warning_Show("Level QTask ID #%d not defined (AI Activate)");
            *(source_val + 0xc) = 0xffffffff;
            return data_3;
        }

        context_val = 1;
    }

    void* data_5 = Script_ExecuteLevelScript_4(*(source_val + 8));

    if (!data_5)
    {
        int item_18_4 = *(source_val + 8);
        int data_6 = Warning_Show("Level QTask ID #%d not defined (AI Activate)");
        *(source_val + 0xc) = 0xffffffff;
        return data_6;
    }

    *(source_val + 0xc) = 0xffffffff;
    *(source_val + 0x10) = 0xffffffff;
    int* data_7 = AI_CheckSector_4(data_5);

    if (!data_7)
    {
        if (context_val)
            data_7 = AI_CheckSector_4(item_4);

        if (!context_val || !data_7)
        {
            int data_8 = DynObj_LookupHashItem_5();
            short item_18_7 = data_8;
            data_8 = *(data_5 + 0x1c);
            data_7 = TaskType_IsDerivedFrom(data_8, item_18_7);

            if (data_7)
            {
                Physics_ApplyImpulse_8(&context, dest_val[0xedb], data_5 + 0x20);
                data_7 = context;

                if (data_7)
                {
                    arg2 = 0xffffffff;

                    if (FX_FireBallUpdate_20(dest_val, data_7, 0xa, sub_48e7a0, &arg2))
                    {
                        void* const entity_7 = arg2;
                        int data_10 = *context;

                        if (data_10 != entity_7)
                        {
                            *(source_val + 0xc) = data_10;
                            *(source_val + 0x10) = arg2;
                        }
                        else
                            *(source_val + 0xc) = entity_7;
                    }

                    data_7 = AI_UpdatePatrol_5(data_5, 1);
                    dest_val[0xeac] = data_7;

                    if (context_val)
                    {
                        data_7 = AI_UpdatePatrol_5(item_4, 1);
                        dest_val[0xead] = data_7;
                    }
                }
            }
        }
    }

    return data_7;
}
