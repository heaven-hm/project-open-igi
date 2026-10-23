// IGI native provenance: Display_08X04X04X02X02X02X02 - 0x00492370 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00492370-Display_08X04X04X02X02X02X02.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_08X04X04X02X02X02X02.h"
int Display_08X04X04X02X02X02X02(char* context, int arg2, short arg3, short arg4 , char arg5, char arg6 , char arg7 , char arg8 , char arg9, char arg10 , char arg11 , char arg12 )

{
    unsigned int item_4 = arg12;
    unsigned int item_8 = arg11;
    unsigned int item_c = arg10;
    unsigned int item_10 = arg9;
    unsigned int item_14 = arg8;
    unsigned int item_18 = arg7;
    unsigned int item_1c = arg6;
    unsigned int item_20 = arg5;
    unsigned int item_24 = arg4;
    unsigned int item_28 = arg3;
    int item_2c = arg2;
    return sprintf(context, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}");
}
