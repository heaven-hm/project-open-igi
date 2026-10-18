// IGI native provenance: Camera_ApplyHandiCam_19 - 0x004AA16D - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AA16D-Camera_ApplyHandiCam_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AA16D-Camera_ApplyHandiCam_19.hpp"
int Camera_ApplyHandiCam_19(char context, int arg2, char arg3)

{
    unsigned int data_2 = context;

    if (!(*(data_2 + 0xc32321) & arg3))
    {
        int result;

        if (!arg2)
            result = 0;
        else
            result = g_54407a[data_2] & arg2;

        if (!result)
            return result;
    }

    return 1;
}
