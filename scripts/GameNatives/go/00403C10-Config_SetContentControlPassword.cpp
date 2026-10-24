// IGI native provenance: Config_SetContentControlPassword - 0x00403C10 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00403C10-GO_ContentControlPW.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00403C10-Config_SetContentControlPassword 2.hpp"
int Config_SetContentControlPassword(int context, float arg2 )

// Sig: int Config_SetContentControlPassword()
// Note: Menu-script setter form (argc=0): same address registered twice by
// igi.exe - site 0x4034C1 pushes str "Config_SetContentControlPassword" ->
// registrar 0x4B8890. Password comes from the script context, not an explicit
// arg. Returns 1.
{
    char* item_24 = 0x20;
    char* item_28 = &g_bc79c8;
    int item_2c = 0;
    float item_30 = arg2;
    Sys_ResolveSymbol_23(item_30, item_2c, item_28, item_24);
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
