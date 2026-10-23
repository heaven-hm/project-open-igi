// IGI native provenance: DefineGroup - 0x004EBA90 - CFG score 97.6% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EBA90-Define_Group.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004EBA90-Define_Group.hpp"
int DefineGroup(int context, void* arg2, float arg3)

// Sig: void DefineGroup(...)
// Note: Defines a GROUP entry (sound-group/entity grouping) during definition
// parsing. Parser-context only.
{
    int context_val = 1;
    void item_60;
    void* flags = &item_60;
    char item_7c = 1;
    int item_84 = 1;
    void item_3e;
    char* kind = Sys_ResolveSymbol_23(arg3, 0, &item_3e, 0x20);
    kind = g_54db4a;
    char item_40 = 1;
    struct status_msg* data_val = QTask_HandleInput(arg2, kind, &flags);
    Script_SetSymbolContext("DefineGroup", data_val);
    Script_SetSymbolContext("DefineSound", data_val);
    Script_SetSymbolContext("DefineGraph", data_val);
    Script_SetSymbolContext("DefineTriggerOnce", data_val);

    if (Sys_AllocateMemory_24(arg3, 1))
    {
        int i;

        do
        {
            Script_GetArg(arg3, context_val);
            context_val += 1;
            i = Sys_AllocateMemory_24(arg3, context_val);
        } while (i);
        item_84 = context_val;
    }

    Script_SetSymbolContext("DefineGroup", arg2);
    Script_SetSymbolContext("DefineSound", arg2);
    Script_SetSymbolContext("DefineGraph", arg2);
    Script_SetSymbolContext("DefineTriggerOnce", arg2);
    double item_70 = item_84;
    int item_78 = context_val;
    char* item_68 = &g_567c74;
    memcpy(context, &item_78, 0x18);
    return context;
}
