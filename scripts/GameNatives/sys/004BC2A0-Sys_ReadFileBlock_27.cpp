// IGI native provenance: Sys_ReadFileBlock_27 - 0x004BC2A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BC2A0-Sys_ReadFileBlock_27.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BC2A0-Sys_ReadFileBlock_27.hpp"
int Sys_ReadFileBlock_27(char* context, int arg2, int arg3)

{
    int item_4 = arg3;
    int item_8 = arg2;
    return sprintf(context, "%s%p");
}
