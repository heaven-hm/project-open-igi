// IGI native provenance: Camera_ApplyLineFilter_32 - 0x004AF7A6 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004AF7A6-Camera_ApplyLineFilter_32.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyLineFilter_32.h"
HRESULT __stdcall AVIStreamRead(struct IAVIStream pavi, int lStart, int lSamples, void* lpBuffer, int cbBuffer, int* plBytes, int* plSamples)

{
    /* tailcall */
    return AVIStreamRead(pavi, lStart, lSamples, lpBuffer, cbBuffer, plBytes, plSamples);
}
