// IGI native provenance: Display_GetDDErrStr - 0x00492B70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00492B70-Display_GetDDErrStr.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_GetDDErrStr.h"

int __stdcall Display_GetDDErrStr(void* self, char* args)

{
    int result_val = *(self + 4);

    if ((result_val & 0x4400) == 0x4400)
    {
        int status = *(self + 0xc);
        char* result = args;
        Game_DataSymbolLoad(args, "%s    %d bits z and stencil\n");
        return 1;
    }

    if ((result_val & 0x400) == 0x400)
    {
        int status = *(self + 0xc);
        char* result = args;
        Game_DataSymbolLoad(args, "%s    %d bits z\n");
    }

    return 1;
}
