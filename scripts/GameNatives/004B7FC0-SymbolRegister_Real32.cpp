// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// IGI native provenance: SymbolRegister_Real32 - 0x004B7FC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7FC0-SymbolRegister_Real32.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SymbolRegister_Real32.h"
void* SymbolRegister_Real32(int* context, int arg2)

// Sig: void* SymbolRegister_Real32(int* table, char* name)
// Note: Registers a REAL32 (float)-typed script variable slot. Params: table,
// name. RETURNS symbol entry. Pairs with AIFunction_Set/GetScriptRealValue for
// float storage.
{
    return Q_TaskHashTableSet(context, Sys_DispatchOpcode_22, arg2, 8, 0, 2, 1);
}
