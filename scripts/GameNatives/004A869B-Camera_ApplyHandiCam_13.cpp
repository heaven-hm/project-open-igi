// IGI native provenance: Camera_ApplyHandiCam_13 - 0x004A869B - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A869B-Camera_ApplyHandiCam_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A869B-Camera_ApplyHandiCam_13.hpp"
long double Camera_ApplyHandiCam_13(double context, short arg2 , int arg3) {
    int entity_val;
    int item_8 = entity_val;
    int item_c = entity_val;
    item_c = context;
    *(&item_8 + 2) = (arg3 + 0x3fe) << 4 | (arg2 & 0x800f);
    return item_c;
}
