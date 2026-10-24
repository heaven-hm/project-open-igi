// IGI native provenance: Camera_ApplyLineFilter_17 - 0x004A9A41 - CFG score 53.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004A9A41-Camera_ApplyLineFilter_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9A41-Camera_ApplyLineFilter_17.hpp"
int Camera_ApplyLineFilter_17(void* context , long double arg2 ) {
    *(context - 0x90) = 0xfe;
    int entity_val;
    *(&entity_val + 1) = 0;
    long double fpu1;
    long double fpu2;
    /* tailcall */
    return sub_4a9a4e(context, fpu1, fpu2);
}
