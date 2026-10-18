// IGI native provenance: Camera_ApplyWideScreen_31 - 0x004AF782 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004AF782-Camera_ApplyWideScreen_31.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AF782-Camera_ApplyWideScreen_31.hpp"
HRESULT __stdcall AVIStreamInfoA(struct IAVIStream pavi, struct AVISTREAMINFOA* psi, int lSize)

{
    /* tailcall */
    return AVIStreamInfoA(pavi, psi, lSize);
}
