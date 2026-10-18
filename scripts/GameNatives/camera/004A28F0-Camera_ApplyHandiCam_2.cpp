// IGI native provenance: Camera_ApplyHandiCam_2 - 0x004A28F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A28F0-Camera_ApplyHandiCam_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A28F0-Camera_ApplyHandiCam_2.hpp"
int Camera_ApplyHandiCam_2()

{
    int result = g_543d54;

    if (result >= 9)
    {
        g_543d54 = 1;
        return result;
    }

    g_543d54 = result + 1;
    return result + 1;
}
