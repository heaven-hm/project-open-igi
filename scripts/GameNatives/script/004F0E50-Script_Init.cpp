// IGI native provenance: Script_Init - 0x004F0E50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0E50-Script_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F0E50-Script_Init.hpp"
int Script_Init(int context, char arg2, int arg3, char arg4)

// Sig: void Script_Init(char* file_name,char p2,int p3,char p4)
// Note: Initialises the script system for a file. Params: file_name = script
// path, p2 = byte flag (observed), p3 = int option, p4 = byte flag. Refined
// types from decompiled header (two char slots detected).
{
    int result = context;
    g_a70c98 = result;
    result = arg4;
    g_54dd16 = arg2;
    g_a70c74 = arg3;
    g_a758bc = result;
    g_a758a8 = 0;
    return result;
}
