// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// IGI native provenance: SymbolRegister_Int16 - 0x004B7E80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7E80-SymbolRegister_Int16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SymbolRegister_Int16.h"
void* SymbolRegister_Int16(int* context, int arg2)

// Sig: void* SymbolRegister_Int16(int* table, char* name)
// Note: Registers an INT16-typed script variable slot. Params: table, name.
// RETURNS symbol entry. Sibling of SymbolRegister_Bool8/Int32/Real32 - same
// thunk shape, different type tag.
{
    return Q_TaskHashTableSet(context, Sys_OpenFileStream_22, arg2, 4, 0, 1, 1);
}
