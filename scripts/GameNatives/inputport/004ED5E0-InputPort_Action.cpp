// IGI native provenance: InputPort_Action - 0x004ED5E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ED5E0-InputPort_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004ED5E0-InputPort_Action.hpp"
int InputPort_Action(int context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8 , int arg9, int arg10, int arg11, int arg12, int arg13, int arg14, int arg15)

{
    int data_1 = context * 0xb0;
    *(data_1 + 0xbc20a4) = arg2;
    *(data_1 + 0xbc20a8) = arg3;
    *(data_1 + 0xbc20ac) = arg4;
    *(data_1 + 0xbc20b0) = 0;
    *(data_1 + 0xbc20b4) = 0;
    *(data_1 + 0xbc20b8) = 0;
    *(data_1 + 0xbc20c0) = arg5;
    *(data_1 + 0xbc20c4) = arg5;
    *(data_1 + 0xbc20c8) = arg10;
    *(data_1 + 0xbc20cc) = arg13;
    *(data_1 + 0xbc20d0) = arg6;
    *(data_1 + 0xbc20d4) = arg8;
    *(data_1 + 0xbc20d8) = arg11;
    *(data_1 + 0xbc20dc) = arg14;
    *(data_1 + 0xbc20e0) = arg7;
    *(data_1 + 0xbc20e4) = arg9;
    *(data_1 + 0xbc20e8) = arg12;
    *(data_1 + 0xbc20ec) = arg15;
    Physics_ApplyImpulse_6(data_1 + &g_bc20f0, SoundSys_StopSoundSample_17);
    return 0;
}
