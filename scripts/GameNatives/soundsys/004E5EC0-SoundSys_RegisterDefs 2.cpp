// IGI native provenance: SoundSys_RegisterDefs - 0x004E5EC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E5EC0-SoundSys_RegisterDefs.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_RegisterDefs.h"

int* SoundSys_RegisterDefs(int self, void* args)

{
    void* arg1 = args;
    int i = 0xffffffff;

    while (i)
    {
        bool cond:0_1 = 0 != *arg1;
        arg1 += 1;
        i -= 1;

        if (!cond:0_1)
            break;
    }

    int counter_val = ~i;
    int source_ptr_1;
    int dest_ptr_2;
    dest_ptr_2 = memcpy(self + 0x20, arg1 - counter_val, counter_val & 0xfffffffc);
    memcpy(dest_ptr_2, source_ptr_1, counter_val & 3);
    DynObj_QtasklistIsFull(g_a5e610, self);
    Script_SetSymbolContext("DefineGroup", self);
    Script_SetSymbolContext("DefineSound", self);
    Script_SetSymbolContext("DefineGraph", self);
    Script_SetSymbolContext("DefineTriggerOnce", self);
    int* result = Qvm_Load(args);

    if (!result)
        return result;

    Qvm_Execute(result);
    return Qvm_Cleanup(result);
}
