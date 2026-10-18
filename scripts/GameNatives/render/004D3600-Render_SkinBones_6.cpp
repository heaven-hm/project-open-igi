// IGI native provenance: Render_SkinBones_6 - 0x004D3600 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D3600-Render_SkinBones_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D3600-Render_SkinBones_6.hpp"
int* Render_SkinBones_6(int* context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11, int arg12)

{
    *context = arg2;
    context[7] = arg3;
    context[8] = arg4;
    context[9] = arg5;
    context[0xa] = arg6;
    context[1] = arg7;
    context[2] = arg8;
    context[3] = arg9;
    context[4] = arg10;
    context[5] = arg11;
    context[6] = arg12;
    return context;
}
