// IGI native provenance: Qsc_Compile - 0x004B8410 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B8410-Qsc_Compile.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B8410-Qsc_Compile.hpp"
int Qsc_Compile(void* context)

// Sig: int Qsc_Compile(char* qscPath)
// Note: Compiles a .qsc SCRIPT SOURCE into a .qvm binary: resolves the path
// (fcn_004B5B60), pulls the source through ResourceLoad (0x4B5F00), allocates a
// 0x94-byte compiler context via Mem_Alloc, emits ".qvm" output. Param: qscPath
// = script source path. RETURNS result object/flag.
{
    int result;
    void* entity_val;
    int temp_val;
    result = Resource_FileExists(context);

    if (!result)
        return result;

    void* item_11c_1 = context;
    int item_104;
    int* data_1 = Resource_Load(&item_104, temp_val, entity_val, &item_104);
    int* data_2 = Mem_Alloc(0x94, 4);
    memset(data_2, 0, 0x94);
    data_2[0x20] = data_1;
    data_2[0x21] = item_104;
    void* dest_2 = context;
    int i = 0xffffffff;
    data_2[0x22] = 0;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_2;
        dest_2 += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_2 = ~i;
    int source_3;
    int dest_5;
    dest_5 = memcpy(data_2, dest_2 - entity_2, entity_2 & 0xfffffffc);
    memcpy(dest_5, source_3, entity_2 & 3);
    void* dest_6 = context;
    int i_1 = 0xffffffff;

    while (i_1)
    {
        bool flag_1_1 = 0 != *dest_6;
        dest_6 += 1;
        i_1 -= 1;

        if (!flag_1_1)
            break;
    }

    int entity_6 = ~i_1;
    void item_80;
    int source_5;
    int dest_8;
    dest_8 = memcpy(&item_80, dest_6 - entity_6, entity_6 & 0xfffffffc);
    memcpy(dest_8, source_5, entity_6 & 3);
    char* data_4 = String_FindExtension(nullptr, 0, &item_80);
    void* dest_9 = context;
    *data_4 = g_547290[0];
    data_4[4] = g_547290[4][0];
    int i_2 = 0xffffffff;

    while (i_2)
    {
        bool flag_2_1 = 0 != *dest_9;
        dest_9 += 1;
        i_2 -= 1;

        if (!flag_2_1)
            break;
    }

    int entity_12 = ~i_2;
    void item_100;
    int source_7;
    int dest_11;
    dest_11 = memcpy(&item_100, dest_9 - entity_12, entity_12 & 0xfffffffc);
    memcpy(dest_11, source_7, entity_12 & 3);
    char* data_6 = String_FindExtension(nullptr, 0, &item_100);
    *data_6 = g_547288[0];
    data_6[4] = g_547288[4][0];
    QScript_Cleanup(&item_80);
    QScript_Cleanup(&item_100);

    if (QScript_ParseAndAssemble(&item_100, data_2))
    {
        Qvm_Cleanup(data_2);
        void* saved_index_3 = context;
        return Sys_FreeMemory_22("An error occured when compiling file: %s");
    }

    if (QScript_Assemble(&item_80, &item_100))
    {
        void* saved_source_3 = context;
        return Sys_FreeMemory_22("An error occured when compiling file: %s");
    }

    QScript_Cleanup(&item_100);
    return Qvm_Cleanup(data_2);
}
