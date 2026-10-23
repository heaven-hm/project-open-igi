// IGI native provenance: Camera_SetFieldOfView_32 - 0x004AF7A0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004AF7A0-Camera_SetFieldOfView_32.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_SetFieldOfView_32.h"
unsigned int __stdcall ICDecompress(HIC hic, unsigned int dwFlags, BITMAPINFOHEADER* lpbiFormat, void* lpData, BITMAPINFOHEADER* lpbi, void* lpBits)

{
    /* tailcall */
    return Camera_SetFieldOfView_32(hic, dwFlags, lpbiFormat, lpData, lpbi, lpBits);
}
