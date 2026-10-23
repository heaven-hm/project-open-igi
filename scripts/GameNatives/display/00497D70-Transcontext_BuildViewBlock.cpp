// Native: Transcontext_BuildViewBlock; retail VA 0x00497D70; signature x86 cdecl int32_t(void*, const void*, const void*, float).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja + Radare2 CFG (docs/fidelity/evidence/0x00497D70-radare2.json,
// -binaryninja.json, -semantic-review.json); angr differential run by scripts/view_block_diff_runner.py.
// Behavior/CFG: one basic block, no branches. Copies the 40-byte view block with rep movsd x10, transposes
// its leading 3x3 dword matrix through the stack (order 0,3,6,1,4,7,2,5,8,9), copies the three camera
// float64 at +0x28 and fovX at +0x40. The stack argument slot is reused for the temporary:
// fld [0x00BCABC4] (height); fmul fovX; fstp [esp+0x44]; call Display_ComputeScreenAspect; fmul [0x00BCABC0]
// (width); fdivr [esp+0x44]; fstp [ctx+0x44] (vertical FOV). Then fld [0x00BCABC8] (half width); fdiv
// [ctx+0x40]; fstp [ctx+0x48] (screen FOVX), and the same aspect product for [ctx+0x4C] (screen FOVY).
// Origins are [0x00BCABB8]+[0x00BCABC8] and [0x00BCABBC]+[0x00BCABCC] into +0x50/+0x54; +0x58 and +0x60
// are set to 1 and the body returns 1.
// Status/limits: canonical reference transcription for the record's engine implementation
// engine/src/retail/render/view_block.cpp. Not compiled into the candidate build; the engine body is the
// translation unit the candidate links, and this file is the address-qualified retail reference.
extern "C" double Display_ComputeScreenAspect();

int Transcontext_BuildViewBlock(void* context, const void* view_source, const void* camera_source, float fov_x)
{
    unsigned char* ctx = static_cast<unsigned char*>(context);
    const unsigned int* source = static_cast<const unsigned int*>(view_source);
    // rep movsd x10 then the transposed copy back.
    unsigned int block[10];
    for (int index = 0; index < 10; ++index)
    {
        block[index] = source[index];
    }
    const unsigned int order[10] = {0, 3, 6, 1, 4, 7, 2, 5, 8, 9};
    for (int index = 0; index < 10; ++index)
    {
        *reinterpret_cast<unsigned int*>(ctx + index * 4) = block[order[index]];
    }
    const double* camera = static_cast<const double*>(camera_source);
    for (int index = 0; index < 3; ++index)
    {
        *reinterpret_cast<double*>(ctx + 0x28 + index * 8) = camera[index];
    }
    *reinterpret_cast<float*>(ctx + 0x40) = fov_x;

    const float scaled_fov_x = *reinterpret_cast<const float*>(0x00BCABC4) * fov_x;
    const double divisor = Display_ComputeScreenAspect() * *reinterpret_cast<const float*>(0x00BCABC0);
    *reinterpret_cast<float*>(ctx + 0x44) = static_cast<float>(scaled_fov_x / divisor);
    *reinterpret_cast<float*>(ctx + 0x48) =
        *reinterpret_cast<const float*>(0x00BCABC8) / *reinterpret_cast<const float*>(ctx + 0x40);
    *reinterpret_cast<float*>(ctx + 0x4C) =
        static_cast<float>(Display_ComputeScreenAspect() * *reinterpret_cast<const float*>(ctx + 0x48));
    *reinterpret_cast<float*>(ctx + 0x50) = *reinterpret_cast<const float*>(0x00BCABB8)
        + *reinterpret_cast<const float*>(0x00BCABC8);
    *reinterpret_cast<float*>(ctx + 0x54) = *reinterpret_cast<const float*>(0x00BCABBC)
        + *reinterpret_cast<const float*>(0x00BCABCC);
    *reinterpret_cast<int*>(ctx + 0x58) = 1;
    *reinterpret_cast<int*>(ctx + 0x60) = 1;
    return 1;
}
