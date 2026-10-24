// IGI native provenance: Camera_ApplyBandFilter_4 - 0x004A5330 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5330-Camera_ApplyBandFilter_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A5330-Camera_ApplyBandFilter_4.hpp"
int* Camera_ApplyBandFilter_4(PSTR context, char* arg2, int arg3)

{
    int* result = Camera_ApplyLineFilter_10();

    if (result)
        return Camera_SetFieldOfView_10(context, arg2, arg3, result);

    return result;
}
