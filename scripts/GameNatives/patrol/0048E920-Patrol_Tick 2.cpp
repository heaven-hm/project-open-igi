// IGI native provenance: Patrol_Tick - 0x0048E920 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E920-Patrol_Tick.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Patrol_Tick.h"
int* Patrol_Tick(int* context, int* arg2)

{
    int source_val = context;
    int data_val = *(source_val + 0x1514);
    void* data_1 = PatrolPath_Action(source_val + data_val * 0x1c + 0x14fc);

    if (*(source_val + data_val * 0x1c + 0x1510))
        AI_WriteChannel(source_val, 1, 1.0f);

    int* result;
    int entity_2;

    if (!*(source_val + data_val * 0x1c + 0x1511))
        result = AI_SelectTarget_3(source_val);
    else
        result = AI_AlertAllies_3(source_val);

    if (data_1)
    {
        int data_3 = PatrolPathCommand_Process(data_1);
        result = GameScreen_Process(data_1);

        switch (data_3)
        {
            case 0:
            {
                int* result_3 = result;
                char* item_30_4 = "Patrolling, Standaction : %d";
                int item_34_3 = 0xffffffff;
                int item_38_2 = source_val;

                if (*(source_val + data_val * 0x1c + 0x1514) == 0xffffffff)
                {
                    int item_2c_5 = entity_2;
                    *(source_val + data_val * 0x1c + 0x1514) = 0;
                    return AI_WriteChannel(source_val, 3, result);
                }

                int item_2c_7;

                if (!*(source_val + data_val * 0x1c + 0x1510))
                    item_2c_7 = 0;
                else
                    item_2c_7 = 1;

                result = Locomotion_GetState(source_val, item_2c_7);

                if (result)
                {
                    result = PatrolCursor_Advance(source_val + data_val * 0x1c + 0x14fc);
                    *(source_val + data_val * 0x1c + 0x1514) = 0xffffffff;
                }
                break;
            }
            case 1:
            {
                if (*(source_val + data_val * 0x1c + 0x1514) == 0xffffffff)
                    *(source_val + data_val * 0x1c + 0x1514) = Frames_GetUpdateCounter() + result;

                unsigned int data_7 = Frames_GetUpdateCounter();
                int item_2c_4 = *(source_val + data_val * 0x1c + 0x1514) - data_7;
                char* item_30_3 = "Patrolling, Delay : %d";
                int item_34_2 = 0xffffffff;
                int item_38_1 = source_val;
                result = Frames_GetUpdateCounter();

                if (result > *(source_val + data_val * 0x1c + 0x1514))
                {
                    result = PatrolCursor_Advance(source_val + data_val * 0x1c + 0x14fc);
                    *(source_val + data_val * 0x1c + 0x1514) = 0xffffffff;
                }
                break;
            }
            case 2:
            case 3:
            {
                int item_3c_2;
                int item_38_4;

                if (data_3 == 2)
                {
                    *(source_val + data_val * 0x1c + 0x1510) = 0;
                    *(source_val + data_val * 0x1c + 0x1511) = 1;
                    int item_2c_9 = *(source_val + 0x3e2c);
                    int* result_2 = result;
                    char* item_34_7 = "Patrolling, Walkto:%d, Cur:%d";
                    item_38_4 = 0xffffffff;
                    item_3c_2 = source_val;
                }
                else if (data_3 == 3)
                {
                    *(source_val + data_val * 0x1c + 0x1510) = 0;
                    *(source_val + data_val * 0x1c + 0x1511) = 0;
                    int item_2c_8 = *(source_val + 0x3e2c);
                    int* result_1 = result;
                    char* item_34_6 = "Patrolling, Runto:%d, Cur:%d";
                    item_38_4 = 0xffffffff;
                    item_3c_2 = source_val;
                }
                result = AI_MoveToNode_3(source_val, result, nullptr, 1);

                if (result)
                {
                    result = PatrolCursor_Advance(source_val + data_val * 0x1c + 0x14fc);
                    *(source_val + data_val * 0x1c + 0x1514) = 0xffffffff;
                }
                break;
            }
            case 4:
            {
                *(source_val + data_val * 0x1c + 0x1510) = 1;
                result = PatrolCursor_Advance(source_val + data_val * 0x1c + 0x14fc);
                *(source_val + data_val * 0x1c + 0x1514) = 0xffffffff;
                break;
            }
            case 5:
            {
                int* result_4 = result;
                char* item_30_6 = "Patrolling, LookAtNode : %d";
                int item_34_5 = 0xffffffff;
                int item_38_3 = source_val;
                double item_18[0x3];
                Script_CallNativeHandler_23(&item_18, Script_ExecuteLevelScript_23(*(source_val + 0x3b6c), result));
                result = AI_TakeCover_3(source_val, &item_18, 0.052359879f, 0);

                if (result)
                {
                    result = PatrolCursor_Advance(source_val + data_val * 0x1c + 0x14fc);
                    *(source_val + data_val * 0x1c + 0x1514) = 0xffffffff;
                }
                break;
            }
            case 7:
            {
                return AI_CombatTarget_2(source_val, arg2);
                break;
            }
        }
    }

    return result;
}
