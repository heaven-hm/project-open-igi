// IGI native provenance: Camera_UpdateTransform_32 - 0x004AF79A - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004AF79A-Camera_UpdateTransform_32.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_UpdateTransform_32.h"
HIC __stdcall ICLocate(unsigned int fccType, unsigned int fccHandler, BITMAPINFOHEADER* lpbiIn, BITMAPINFOHEADER* lpbiOut, unsigned short wFlags)

{
    /* tailcall */
    return Camera_UpdateTransform_32(fccType, fccHandler, lpbiIn, lpbiOut, wFlags);
}
