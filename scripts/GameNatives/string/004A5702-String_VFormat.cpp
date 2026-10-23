// IGI native provenance: String_VFormat - 0x004A5702 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5702-String_VFormat.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "String_VFormat.h"
int String_VFormat(char* context, char* arg2, int arg3)

// Sig: int String_VFormat(char* buffer, const char* format, va_list args)
// Note: Bounded printf-style va_list formatter wrapper around the parser core
// at 0x004A7720. Catalogued for direct ABI-aware use; no generic NativeHelper
// wrapper is emitted. Ghidra/r2 CFG agreement: 4 basic blocks and 4 edges; see
// assets/IGI1-Native-Name-Evidence.json.
{
    int item_2c = arg3;
    char* item_1c = context;
    char* item_24 = context;
    int item_18 = 0x42;
    int item_20 = 0x7fffffff;
    int result = _output(&item_24, arg2);
    int item_20_1 = item_20 - 1;

    if (item_20 - 1 < 0)
    {
        int item_30_1 = 0;
        _flsbuf(&item_24);
    }
    else
        *item_24 = 0;

    return result;
}
