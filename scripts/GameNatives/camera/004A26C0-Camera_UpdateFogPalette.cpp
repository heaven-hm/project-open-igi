// IGI native provenance: Camera_UpdateFogPalette - 0x004A26C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A26C0-Camera_UpdateFogPalette.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A26C0-Camera_UpdateFogPalette.hpp"
extern "C" {
extern float g_935c10[16];
}
void Camera_UpdateFogPalette(void *context, float *arg2)
{
    float scale = *(float *)((char *)arg2 + 0x64) * *(float *)0x5342d8;
    g_935c10[0] = scale;
}
