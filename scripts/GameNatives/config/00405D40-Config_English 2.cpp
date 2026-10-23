// IGI native provenance: Config_English - 0x00405D40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00405D40-Config_English.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Config_English.h"
int Config_English(int* context, int arg2)

{
    void* item_10c = &"ENGLISH"[(&g_bc238c)[arg2 * 0x159] * 2];
    char const* const item_110 = "GOGameLang";
    char* item_114 = "%s(%s);\n";
    void item_100;
    char* item_118 = &item_100;
    int data_7 = sprintf(item_118, item_114);
    void* item_11c = &g_53337c[(&g_bc2388)[arg2 * 0x159] << 4];
    char const* const item_120 = "GOGameDiff";
    int dest_1 = data_7 + sprintf(&item_118 + data_7 + 0x18, "%s(%s);\n");
    char* data_11 = "TRUE";

    if (!(&g_bc2390)[arg2 * 0x564])
        data_11 = "FALSE";

    char* item_10c_1 = data_11;
    item_110 = "GOIsBlood";
    char* item_114_1 = "%s(%s);\n";
    item_118 = &item_110 + dest_1 + 0x10;
    return Sys_CloseFileStream_4(context, &item_100, dest_1 + sprintf(item_118, item_114_1));
}
