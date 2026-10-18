// IGI native provenance: DynObj_ParseConfigFile_11 - 0x004CE390 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CE390-DynObj_ParseConfigFile_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004CE390-DynObj_ParseConfigFile_11.hpp"
unsigned int DynObj_ParseConfigFile_11(int context, char arg2, int arg3, double* arg4, int arg5)

{
    unsigned int result;

    for (int i = 0; i < 6; i += 1)
        result = DynObj_CreateMagicObject_11(context, arg2, arg3, arg4, arg5, i);

    return result;
}
