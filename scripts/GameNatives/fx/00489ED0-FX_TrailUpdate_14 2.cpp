// IGI native provenance: FX_TrailUpdate_14 - 0x00489ED0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00489ED0-FX_TrailUpdate_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_TrailUpdate_14.h"
int FX_TrailUpdate_14(int context, int arg2, int arg3, float arg4)

{
    return Render_BindTexture_9(context + 0x68, 1, 5, context + 0x650, arg2, arg3, arg4);
}
