// IGI native provenance: Q_TaskHashTableSet - 0x004BAAC0 - CFG score 93.3% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BAAC0-Q_TaskHashTableSet.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Q_TaskHashTableSet.h"
void* Q_TaskHashTableSet(int* context, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7)

// Sig: void Q_TaskHashTableSet(int** table, void* handler, int nameVal, int p4,
// int p5, int p6, int p7)
// Note: THE CORE REGISTRAR: inserts an entry into the task/hash table binding a
// name-value pair to a handler callback. All 7 stack args confirmed by
// disassembly header (arg_18h..arg_3Ch). Every SymbolRegister* thunk (e.g.
// 0x4B7EE0 pushes 1,1,0,1,addr,handler,table -> this) and every
// GAME_DEFINE_OPTIONS registration routes here. Params: table = target hash
// table, handler = callback invoked for the symbol, nameVal = name/value ref,
// p4-p7 = type/flag slots (see SymbolRegister thunks for known values).
{
    Qhash_Init(1);
    int* data_val = Mem_Alloc(0x1c, 4);
    memset(data_val, 0, 0x1c);
    data_val[5] = Symbol_CheckRegister(g_a43e88, context, data_val);
    data_val[4] = arg2;
    data_val[6] = arg3;
    data_val[3] = arg4;
    *data_val = arg5;
    data_val[1] = arg6;
    data_val[2] = arg7;
    return Qhash_Reset();
}
