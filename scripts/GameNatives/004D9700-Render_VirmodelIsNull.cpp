// IGI native provenance: Render_VirmodelIsNull - 0x004D9700 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D9700-Render_VirmodelIsNull.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004D9700-Render_VirmodelIsNull.hpp"
int Render_VirmodelIsNull(void* context, int* arg2)

{
    if (arg2)
    {
        *(context + 0x74) = arg2;
        int result;
        long double fp0;
        fp0 = Render_ComputeNormals(arg2);
        *(context + 0xa0) = fp0;
        return result;
    }

    Error_Show("VirModel is null");

    while (true)
        /* nop */
}
