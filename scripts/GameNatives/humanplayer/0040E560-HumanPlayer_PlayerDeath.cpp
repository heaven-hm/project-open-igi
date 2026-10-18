// IGI native provenance: HumanPlayer_PlayerDeath - 0x0040E560 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040E560-HumanPlayer_PlayerDeath.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlayerDeath.hpp"
int HumanPlayer_PlayerDeath(void* context, void* arg2)

{
    *(arg2 + 0x33d) = 0;
    Bones_PlayAnimation(arg2 + 0x68, 0,
        *(FX_FragmentSimulate_14(arg2 + 0x254, *(arg2 + 0x2e8)) + (Sys_FreeMemory_14(Level_SetMission_5(), 2) << 2)
            + 8),
        0, 0.200000003f);
    HumanPlayer_CheckCollision_9(arg2, 0.0f);
    FX_TrailUpdate_15(arg2);
    HumanPlayer_UpdateAnimation_4(arg2);
    Human_ViewCam(arg2, 5);
    AI_MoveToNode_22(*(arg2 + 0x4ec));
    AI_TakeCover_20(arg2);
    AI_TakeCover_21(arg2, "player_death_", 3);
    *(arg2 + 0x664) = *(arg2 + 0x688);
    *(arg2 + 0x668) = *(arg2 + 0x68c);
    int result = *(arg2 + 0x690);
    *(arg2 + 0x66c) = result;
    *(context + 0x14) = HumanPlayer_LoadParameters_8;
    return result;
}
