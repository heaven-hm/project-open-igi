// IGI native provenance: Render_ComputeNormals_12 - 0x004DA180 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004DA180-Render_ComputeNormals_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004DA180-Render_ComputeNormals_12.hpp"
int Render_ComputeNormals_12(void** context, int* arg2)

{
    void* dest_val = (g_a5e300 << 2) + &g_a546bc;
    *context = dest_val;
    memcpy(dest_val, &g_a54694, g_a5e2fc << 2);
    *arg2 = g_a5e2fc;
    int result = g_a5e300 + g_a5e2fc;
    g_a5e300 = result;
    return result;
}
