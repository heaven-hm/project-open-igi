// IGI native provenance: Game_DefineOptions - 0x004B8890 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B8890-Game_DefineOptions.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Game_DefineOptions.h"
void* Game_DefineOptions(int* context, int arg2, int arg3, int arg4)

// Sig: void Game_DefineOptions(int **option_str,int option_func_addr,int
// opt1,int opt2)
// Note: Define game options constants , Params: opt_str (int**)'DefineAmmoType'
{
    return Q_TaskHashTableSet(context, arg2, arg3, arg4, 1, 0, 0);
}
