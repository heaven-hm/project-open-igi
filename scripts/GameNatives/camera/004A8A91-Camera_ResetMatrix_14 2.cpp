// IGI native provenance: Camera_ResetMatrix_14 - 0x004A8A91 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A8A91-Camera_ResetMatrix_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ResetMatrix_14.h"
long double Camera_ResetMatrix_14(double context, short arg2 ) {
    if ((arg2 & 0x7ff0) != 0x7ff0)
        return context;

    short item_6_1 = arg2 | 0x7fff;
    int context_1 = context;
    int item_a_1 = *(&context + 4) << 0xb | context_1 >> 0x15;
    return context_1;
}
