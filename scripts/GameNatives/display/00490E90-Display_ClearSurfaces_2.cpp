// IGI native provenance: Display_ClearSurfaces_2 - 0x00490E90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00490E90-Display_ClearSurfaces_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_ClearSurfaces_2.hpp"
HRESULT Display_ClearSurfaces_2(void* context, int arg2, unsigned int arg3, int arg4, int arg5)

{
    HRESULT data_1 =
        AVIStreamRead(*(context + 0x9c), arg2, 1, *(context + 0x160), *(context + 0x140), nullptr, nullptr);

    if (data_1)
    {
        data_1 = 0;
        return data_1;
    }

    unsigned int data_4 = ICDecompress(*(context + 0x15c), arg3, context + 0x12c, *(context + 0x160),
        *(context + 0x154), *(context + 0x164));

    if (data_4)
    {
        data_4 = 0;
        return data_4;
    }

    *(context + 0x1c) = arg2;
    data_4 = 1;
    return data_4;
}
